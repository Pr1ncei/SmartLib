/*
    [SYSTEM IMPLEMENTATION]

    @file: BookManager.cpp
    @author: Gil Miranda and Prince Pamintuan
    @date: March 20,2026

    @brief Implementation of the business logic for managing all books in the application
*/

#include "../books/BookManager.h"
#include "../../utils/Logger.h"
#include "../../core/EventBus.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <stdexcept>

static BookRow rowFromResult(sql::ResultSet* res)
{
    BookRow r;
    r.bookId = res->getInt("book_id");
    r.title = res->getString("title");
    r.author = res->getString("author");
    r.dewey = res->getString("dewey");
    r.totalCopies = res->getInt("total_copies");
    r.availableCopies = res->getInt("available_copies");
    return r;
}

std::vector<BookRow> BookManager::getAllBooks()
{
    std::vector<BookRow> books;
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT * FROM books ORDER BY title"));

        while (res->next())
            books.push_back(rowFromResult(res.get()));
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::getAllBooks] ") + e.what());
    }
    return books;
}

std::vector<BookRow> BookManager::searchBooksGui(const std::string& keyword)
{
    if (keyword.empty()) return getAllBooks();

    std::vector<BookRow> books;
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT * FROM books "
                "WHERE title LIKE ? OR author LIKE ? OR dewey LIKE ? "
                "   OR CAST(book_id AS CHAR) LIKE ? "
                "ORDER BY title"));

        std::string kw = "%" + keyword + "%";
        pstmt->setString(1, kw);
        pstmt->setString(2, kw);
        pstmt->setString(3, kw);
        pstmt->setString(4, kw);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next())
            books.push_back(rowFromResult(res.get()));
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::searchBooksGui] ") + e.what());
    }
    return books;
}

BookStats BookManager::getStats()
{
    BookStats s{};
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT COUNT(*)                           AS total_books, "
            "       COALESCE(SUM(total_copies),     0) AS total_copies, "
            "       COALESCE(SUM(available_copies), 0) AS available_copies "
            "FROM books"));

        if (res->next())
        {
            s.totalBooks = res->getInt("total_books");
            s.totalCopies = res->getInt("total_copies");
            s.availableCopies = res->getInt("available_copies");
            s.borrowedCopies = s.totalCopies - s.availableCopies;
        }
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::getStats] ") + e.what());
    }
    return s;
}

bool BookManager::addBookGui(const std::string& title,
    const std::string& author,
    const std::string& dewey,
    int copies)
{
    if (title.empty() || author.empty() || dewey.empty() || copies < 1)
        return false;

    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO books (title, author, dewey, total_copies, available_copies)"
                " VALUES (?, ?, ?, ?, ?)"));
        pstmt->setString(1, title);
        pstmt->setString(2, author);
        pstmt->setString(3, dewey);
        pstmt->setInt(4, copies);
        pstmt->setInt(5, copies);
        pstmt->execute();

        Logger::getInstance().info("BOOK ADDED (GUI): " + title);
        EventBus::getInstance().publish("BOOK_ADDED", "title=" + title);
        return true;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::addBookGui] ") + e.what());
        return false;
    }
}
bool BookManager::editBookGui(int bookId,
    const std::string& title,
    const std::string& author,
    const std::string& dewey,
    int totalCopies)
{
    try
    {
        sql::Connection* con = db_.getConnection();

        // Fetch current values so empty args fall back gracefully
        std::unique_ptr<sql::PreparedStatement> sel(
            con->prepareStatement(
                "SELECT title, author, dewey, total_copies"
                " FROM books WHERE book_id=?"));
        sel->setInt(1, bookId);
        std::unique_ptr<sql::ResultSet> res(sel->executeQuery());
        if (!res->next()) return false;

        std::string newTitle = title.empty() ? std::string(res->getString("title")) : title;
        std::string newAuthor = author.empty() ? std::string(res->getString("author")) : author;
        std::string newDewey = dewey.empty() ? std::string(res->getString("dewey")) : dewey;
        int         newCopies = (totalCopies < 1) ? res->getInt("total_copies") : totalCopies;

        std::unique_ptr<sql::PreparedStatement> upd(
            con->prepareStatement(
                "UPDATE books SET title=?, author=?, dewey=?, total_copies=?"
                " WHERE book_id=?"));
        upd->setString(1, newTitle);
        upd->setString(2, newAuthor);
        upd->setString(3, newDewey);
        upd->setInt(4, newCopies);
        upd->setInt(5, bookId);

        if (upd->executeUpdate() > 0)
        {
            Logger::getInstance().info(
                "BOOK EDITED (GUI): id=" + std::to_string(bookId));
            EventBus::getInstance().publish("BOOK_EDITED",
                "book_id=" + std::to_string(bookId));
            return true;
        }
        return false;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::editBookGui] ") + e.what());
        return false;
    }
}

bool BookManager::deleteBook(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM books WHERE book_id=?"));
        pstmt->setInt(1, bookId);

        if (pstmt->executeUpdate() > 0)
        {
            Logger::getInstance().info(
                "BOOK DELETED (GUI): id=" + std::to_string(bookId));
            EventBus::getInstance().publish("BOOK_DELETED",
                "book_id=" + std::to_string(bookId));
            return true;
        }
        return false;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::deleteBook] ") + e.what());
        return false;
    }
}

Book BookManager::getBookById(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM books WHERE book_id = ?"));
        pstmt->setInt(1, bookId);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next())
        {
            return Book(
                res->getInt("book_id"),
                res->getString("title"),
                res->getString("author"),
                res->getString("dewey"),
                res->getInt("total_copies"),
                res->getInt("available_copies")
            );
        }
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::getBookById] ") + e.what());
    }

    // Return default Book if not found
    return Book{};
}

bool BookManager::decrementAvailableCopies(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE books SET available_copies = available_copies - 1 "
                "WHERE book_id = ? AND available_copies > 0"));
        pstmt->setInt(1, bookId);
        return pstmt->executeUpdate() > 0;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::decrementAvailableCopies] ") + e.what());
        return false;
    }
}

bool BookManager::incrementAvailableCopies(int bookId)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE books SET available_copies = available_copies + 1 "
                "WHERE book_id = ?"));
        pstmt->setInt(1, bookId);
        return pstmt->executeUpdate() > 0;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[BookManager::incrementAvailableCopies] ") + e.what());
        return false;
    }
}