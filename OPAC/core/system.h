#ifndef SYSTEM_H
#define SYSTEM_H

#include "../db/database.h"

class OpacSystem {
private:
    Database db;

public:
    void run();
    void addBook();
    void viewBooks();
    void updateBook();
    void deleteBook();
};

#endif