#include "../borrow/BorrowManager.h"
#include "../../utils/Logger.h"
#include "../../core/EventBus.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <ctime>
#include <stdexcept>

bool BorrowRow::isOverdue() const
{
    if (!returnDate.empty())
        return overdueDays > 0;
    std::time_t now = std::time(nullptr);
    std::tm* tmNow = std::localtime(&now);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tmNow);
    return dueDate < std::string(buf);
}

static BorrowRow rowFromResult(sql::ResultSet* res)
{
    BorrowRow r;
    r.borrowId = res->getInt("borrow_id");
    r.bookId = res->getInt("book_id");
    r.bookTitle = res->getString("title");
    r.username = res->getString("username");
    r.borrowDate = res->getString("borrow_date");
    r.dueDate = res->getString("due_date");
    r.returnDate = res->isNull("return_date") ? "" : res->getString("return_date");
    r.overdueDays = res->isNull("overdue_days") ? 0 : res->getInt("overdue_days");
    r.fee = res->isNull("fee") ? 0.0 : res->getDouble("fee");
    return r;
}
bool BorrowManager::borrowBookGui(int bookId, const std::string& username)
{
    try
    {
        Book book = bookMgr_.getBookById(bookId);   
        if (!book.isAvailable()) return false;

        sql::Connection* con = db_.getConnection();

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO borrow_records "
                "(book_id, username, borrow_date, due_date) "
                "VALUES (?, ?, NOW(), DATE_ADD(NOW(), INTERVAL ? DAY))"));
        pstmt->setInt(1, bookId);
        pstmt->setString(2, username);
        pstmt->setInt(3, FeeCalculator::BORROW_PERIOD_DAYS);
        pstmt->execute();

        // Roll back the insert if inventory decrement fails
        if (!bookMgr_.decrementAvailableCopies(bookId))
        {
            std::unique_ptr<sql::PreparedStatement> del(
                con->prepareStatement(
                    "DELETE FROM borrow_records "
                    "WHERE book_id=? AND username=? AND return_date IS NULL "
                    "ORDER BY borrow_date DESC LIMIT 1"));
            del->setInt(1, bookId);
            del->setString(2, username);
            del->execute();
            return false;
        }

        Logger::getInstance().info(
            "BORROW (GUI): user=" + username +
            " book_id=" + std::to_string(bookId));
        EventBus::getInstance().publish("BOOK_BORROWED",
            "username=" + username +
            ";book_id=" + std::to_string(bookId));
        return true;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::borrowBookGui] ") + e.what());
        return false;
    }
}

bool BorrowManager::returnBookGui(int bookId,
    const std::string& username,
    const std::string& returnDate,
    bool confirmPayment)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> sel(
            con->prepareStatement(
                "SELECT borrow_id, due_date "
                "FROM borrow_records "
                "WHERE book_id=? AND username=? AND return_date IS NULL "
                "ORDER BY borrow_date DESC LIMIT 1"));
        sel->setInt(1, bookId);
        sel->setString(2, username);
        std::unique_ptr<sql::ResultSet> res(sel->executeQuery());
        if (!res->next()) return false;

        int         borrowId = res->getInt("borrow_id");
        std::string dueDate = res->getString("due_date");

        int    days = FeeCalculator::overdueDays(dueDate, returnDate);
        double fee = FeeCalculator::overdueFee(days);

        if (fee > 0.0 && !confirmPayment) return false;

        std::unique_ptr<sql::PreparedStatement> upd(
            con->prepareStatement(
                "UPDATE borrow_records "
                "SET return_date=?, overdue_days=?, fee=? "
                "WHERE borrow_id=?"));
        upd->setString(1, returnDate);
        upd->setInt(2, days);
        upd->setDouble(3, fee);
        upd->setInt(4, borrowId);
        upd->executeUpdate();

        bookMgr_.incrementAvailableCopies(bookId);

        Logger::getInstance().info(
            "RETURN (GUI): user=" + username +
            " book_id=" + std::to_string(bookId) +
            " overdue_days=" + std::to_string(days) +
            " fee=" + std::to_string(fee));
        EventBus::getInstance().publish("BOOK_RETURNED",
            "username=" + username +
            ";book_id=" + std::to_string(bookId) +
            ";fee=" + std::to_string(fee));
        return true;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::returnBookGui] ") + e.what());
        return false;
    }
}

BorrowRow BorrowManager::getActiveBorrow(int bookId, const std::string& username)
{
    BorrowRow empty{};
    empty.borrowId = -1;
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT br.borrow_id, br.book_id, b.title, br.username, "
                "       br.borrow_date, br.due_date, br.return_date, "
                "       br.overdue_days, br.fee "
                "FROM borrow_records br "
                "JOIN books b ON br.book_id = b.book_id "
                "WHERE br.book_id=? AND br.username=? AND br.return_date IS NULL "
                "ORDER BY br.borrow_date DESC LIMIT 1"));
        pstmt->setInt(1, bookId);
        pstmt->setString(2, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) return rowFromResult(res.get());
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::getActiveBorrow] ") + e.what());
    }
    return empty;
}

std::vector<BorrowRow> BorrowManager::getActiveBorrowsForUser(
    const std::string& username)
{
    std::vector<BorrowRow> rows;
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT br.borrow_id, br.book_id, b.title, br.username, "
                "       br.borrow_date, br.due_date, br.return_date, "
                "       br.overdue_days, br.fee "
                "FROM borrow_records br "
                "JOIN books b ON br.book_id = b.book_id "
                "WHERE br.username=? AND br.return_date IS NULL "
                "ORDER BY br.due_date ASC"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next())
            rows.push_back(rowFromResult(res.get()));
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::getActiveBorrowsForUser] ") + e.what());
    }
    return rows;
}

std::vector<BorrowRow> BorrowManager::getAllActiveBorrows()
{
    std::vector<BorrowRow> rows;
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT br.borrow_id, br.book_id, b.title, br.username, "
            "       br.borrow_date, br.due_date, br.return_date, "
            "       br.overdue_days, br.fee "
            "FROM borrow_records br "
            "JOIN books b ON br.book_id = b.book_id "
            "WHERE br.return_date IS NULL "
            "ORDER BY br.due_date ASC"));
        while (res->next())
            rows.push_back(rowFromResult(res.get()));
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BorrowManager::getAllActiveBorrows] ") + e.what());
    }
    return rows;
}