/*
    [SYSTEM DESIGN/HEADER]

    @file: BookManager.h
    @author: Gil Miranda and Prince Pamintuan 
    @date: March 20,2026

    @brief Business logic for managing all books in the application
*/

#pragma once
#include <string>
#include <vector>
#include "../../db/Database.h"
#include "../../models/Book.h"

namespace sql {
    class ResultSet;
}

// This one represents a single book in record and it's
// used for displaying its details in the dashboard
struct BookRow
{
    int         bookId;
    std::string title;
    std::string author;
    std::string dewey;
    int         totalCopies;
    int         availableCopies;
    bool        isAvailable() const { return availableCopies > 0; }
};

// The statistics above in the dashboard 
// admin can only see this stats
struct BookStats
{
    int totalBooks;       // distinct titles
    int totalCopies;      // SUM(total_copies)
    int availableCopies;  // SUM(available_copies)
    int borrowedCopies;   // totalCopies - availableCopies
};

// Service layer for book operations 
// All of the functions here are also communicating with the database 
class BookManager
{
public:
    void addBook();
    void editBook();
    void deleteBook();
    void viewBooks(const std::string& role);   
    void searchBook();

    Book getBookById(int bookId);
    bool decrementAvailableCopies(int bookId);
    bool incrementAvailableCopies(int bookId);
    void showBookDetails(int bookId);
    std::vector<BookRow> getAllBooks();

    // Returns books matching keyword across title / author / dewey / id.
    // Passing an empty string is equivalent to getAllBooks().
    std::vector<BookRow> searchBooksGui(const std::string& keyword);

    BookStats getStats();

    // GUI Layer Function 
    bool addBookGui(const std::string& title,
        const std::string& author,
        const std::string& dewey,
        int copies);

    // This is the GUI layer function
    // Updates a book by ID. Pass an empty string to keep a text field unchanged;
    // pass totalCopies < 1 to keep the existing copy count.
    // Returns true on success, false if ID not found or DB error.
    bool editBookGui(int bookId,
        const std::string& title,
        const std::string& author,
        const std::string& dewey,
        int totalCopies);

    bool deleteBook(int bookId);

private:
    Database db_;
    void printTableHeader(const std::string& role) const;
    void printBookRow(sql::ResultSet* res, const std::string& role) const;
};