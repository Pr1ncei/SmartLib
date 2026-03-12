#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book {
public:
    int book_id;
    std::string title;
    std::string author;
    int year;

    // Constructor
    Book(int id, std::string t, std::string a, int y);
};

#endif