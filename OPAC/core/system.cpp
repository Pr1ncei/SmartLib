#include "system.h"
#include "../models/Book.h"
#include <iostream>
#include <string>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

void OpacSystem::run() {
    int choice;
    do {
        cout << "\n====== BOOK MANAGEMENT ======\n";
        cout << "1. Add Book\n";
        cout << "2. View Books\n";
        cout << "3. Update Book\n";
        cout << "4. Delete Book\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: addBook(); break;
        case 2: viewBooks(); break;
        case 3: updateBook(); break;
        case 4: deleteBook(); break;
        case 5: cout << "Exiting...\n"; break;
        default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);
}

void OpacSystem::addBook() {
    string title, author;
    int year;

    cin.ignore();
    cout << "Title: ";
    getline(cin, title);
    cout << "Author: ";
    getline(cin, author);
    cout << "Year: ";
    cin >> year;

    sql::Connection* con = db.getConnection();
    if (!con) return;

    sql::Statement* stmt = con->createStatement();
    stmt->execute("INSERT INTO books(title, author, year) VALUES('" +
        title + "', '" + author + "', " + to_string(year) + ")");

    cout << "Book Added Successfully!\n";
    delete stmt;
}

void OpacSystem::viewBooks() {
    sql::Connection* con = db.getConnection();
    if (!con) return;

    sql::Statement* stmt = con->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM books");

    cout << "\nID | Title | Author | Year\n";
    cout << "---------------------------------\n";

    while (res->next()) {
        Book b(res->getInt("book_id"), res->getString("title"), res->getString("author"), res->getInt("year"));

        cout << b.book_id << " | " << b.title << " | " << b.author << " | " << b.year << endl;
    }

    delete res;
    delete stmt;
}

void OpacSystem::updateBook() {
    string title, author;
    int year, book_id;

    cout << "Enter Book ID to update: ";
    cin >> book_id;
    cin.ignore();

    cout << "New Title: ";
    getline(cin, title);
    cout << "New Author: ";
    getline(cin, author);
    cout << "New Year: ";
    cin >> year;

    sql::Connection* con = db.getConnection();
    if (!con) return;

    sql::Statement* stmt = con->createStatement();
    stmt->execute("UPDATE books SET title='" + title +
        "', author='" + author +
        "', year=" + to_string(year) +
        " WHERE book_id=" + to_string(book_id));

    cout << "Book Updated!\n";
    delete stmt;
}

void OpacSystem::deleteBook() {
    int book_id;
    cout << "Enter Book ID to delete: ";
    cin >> book_id;

    sql::Connection* con = db.getConnection();
    if (!con) return;

    sql::Statement* stmt = con->createStatement();
    stmt->execute("DELETE FROM books WHERE book_id=" + to_string(book_id));

    cout << "Book Deleted!\n";
    delete stmt;
}