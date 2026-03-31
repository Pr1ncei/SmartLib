#include "admin_widget.h"
#include "borrow_dialog.h"
#include "../managers/books/BookManager.h"
#include "../managers/borrow/BorrowManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDate>
#include <QFrame>

AdminWidget::AdminWidget(const std::string& username, QWidget* parent)
    : QWidget(parent), username_(username)
{
    buildUI();
    refresh();
}

void AdminWidget::buildUI()
{
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ────────────────────────────────────────────
    auto* sidebar = new QWidget;
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(200);
    auto* sLay = new QVBoxLayout(sidebar);
    sLay->setContentsMargins(0, 0, 0, 0);
    sLay->setSpacing(0);

    // Logo
    auto* logoArea = new QWidget;
    logoArea->setStyleSheet("padding:16px; border-bottom:1px solid #e5e3db; background:#ffffff;");
    auto* logoLay = new QHBoxLayout(logoArea);
    logoLay->setContentsMargins(0, 0, 0, 0);
    auto* logoText = new QVBoxLayout;
    auto* logoName = new QLabel("SmartLib"); logoName->setObjectName("SidebarLogo");
    logoText->addWidget(logoName);
    logoText->setSpacing(0);
    logoLay->addSpacing(8);
    logoLay->addLayout(logoText);
    logoLay->addStretch();
    sLay->addWidget(logoArea);

    auto addSection = [&](const QString& label) {
        auto* lbl = new QLabel(label.toUpper());
        lbl->setObjectName("NavSection");
        sLay->addWidget(lbl);
        };

    auto addNavBtn = [&](const QString& text, QPushButton*& btn) {
        btn = new QPushButton(text);
        btn->setObjectName("NavBtn");
        btn->setCursor(Qt::PointingHandCursor);
        sLay->addWidget(btn);
        };

    addSection("Catalog");
    addNavBtn("All books", navBooks_);
    addNavBtn("Add book", navAddBook_);
    addSection("Circulation");
    addNavBtn("Borrowed books", navBorrowed_);

    sLay->addStretch();

    // Sidebar footer
    auto* footer = new QWidget;
    footer->setObjectName("SidebarFooter");
    auto* fLay = new QHBoxLayout(footer);
    fLay->setContentsMargins(0, 0, 0, 0);
    auto* avatar = new QLabel("AD");
    avatar->setObjectName("AvatarAdmin");
    avatar->setAlignment(Qt::AlignCenter);
    auto* nameLay = new QVBoxLayout;
    auto* uName = new QLabel(QString::fromStdString(username_)); uName->setObjectName("UserName");
    auto* uRole = new QLabel("Administrator");                   uRole->setObjectName("UserRole");
    nameLay->addWidget(uName);
    nameLay->addWidget(uRole);
    nameLay->setSpacing(0);
    fLay->addWidget(avatar);
    fLay->addSpacing(8);
    fLay->addLayout(nameLay);
    fLay->addStretch();

    auto* logoutBtn = new QPushButton("↪");
    logoutBtn->setObjectName("BtnLink");
    logoutBtn->setToolTip("Logout");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    fLay->addWidget(logoutBtn);
    sLay->addWidget(footer);
    root->addWidget(sidebar);

    // ── Main area ─────────────────────────────────────────
    auto* mainArea = new QWidget;
    mainArea->setObjectName("MainArea");
    auto* mLay = new QVBoxLayout(mainArea);
    mLay->setContentsMargins(24, 24, 24, 24);
    mLay->setSpacing(16);

    // Top bar
    auto* topBar = new QHBoxLayout;
    auto* pageTitle = new QLabel("Book catalog");
    pageTitle->setObjectName("PageTitle");
    auto* addBtn = new QPushButton("+ Add book");
    addBtn->setObjectName("BtnPrimary");
    addBtn->setFixedWidth(110);
    addBtn->setCursor(Qt::PointingHandCursor);
    topBar->addWidget(pageTitle);
    topBar->addStretch();
    topBar->addWidget(addBtn);
    mLay->addLayout(topBar);

    // Stats row
    auto* statsRow = new QHBoxLayout;
    statsRow->setSpacing(10);
    auto makeStatCard = [&](const QString& label, QLabel*& valLbl,
        bool warn = false, bool danger = false) {
            auto* card = new QWidget; card->setObjectName("StatCard");
            auto* cLay = new QVBoxLayout(card); cLay->setSpacing(2);
            auto* lbl = new QLabel(label); lbl->setObjectName("StatLabel");
            valLbl = new QLabel("0");   valLbl->setObjectName("StatValue");
            if (warn)   valLbl->setProperty("warn", true);
            if (danger) valLbl->setProperty("danger", true);
            cLay->addWidget(lbl);
            cLay->addWidget(valLbl);
            statsRow->addWidget(card);
        };
    makeStatCard("Total books", statTotal_);
    makeStatCard("Available", statAvail_);
    makeStatCard("Borrowed", statBorrowed_, true);
    makeStatCard("Overdue", statOverdue_, false, true);
    mLay->addLayout(statsRow);

    // Card with search + table
    auto* card = new QWidget; card->setObjectName("Card");
    auto* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(0, 0, 0, 0);
    cardLay->setSpacing(0);

    auto* searchRow = new QHBoxLayout;
    searchRow->setContentsMargins(12, 10, 12, 10);
    searchEdit_ = new QLineEdit;
    searchEdit_->setObjectName("SearchInput");
    searchEdit_->setPlaceholderText("Search by title, author, Dewey, or ID…");
    searchRow->addWidget(searchEdit_);
    cardLay->addLayout(searchRow);

    auto* divLine = new QFrame;
    divLine->setFrameShape(QFrame::HLine);
    divLine->setObjectName("HDivider");
    cardLay->addWidget(divLine);

    // Book table
    bookTable_ = new QTableWidget(0, 6);
    bookTable_->setHorizontalHeaderLabels({ "ID","Title","Author","Dewey","Copies","Status" });
    bookTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    bookTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    bookTable_->setColumnWidth(0, 44);
    bookTable_->setColumnWidth(3, 80);
    bookTable_->setColumnWidth(4, 80);
    bookTable_->setColumnWidth(5, 100);
    bookTable_->setColumnWidth(6, 100); 
    bookTable_->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    bookTable_->verticalHeader()->setVisible(false);
    bookTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bookTable_->setShowGrid(false);
    cardLay->addWidget(bookTable_);
    mLay->addWidget(card);

    root->addWidget(mainArea);

    // ── Connections ────────────────────────────────────────
    connect(navBooks_, &QPushButton::clicked, this, [=] { loadBooks(); });
    connect(navAddBook_, &QPushButton::clicked, this, &AdminWidget::onAddBook);
    connect(navBorrowed_, &QPushButton::clicked, this, &AdminWidget::loadBorrowed);
    connect(addBtn, &QPushButton::clicked, this, &AdminWidget::onAddBook);
    connect(logoutBtn, &QPushButton::clicked, this, &AdminWidget::logoutRequested);
    connect(searchEdit_, &QLineEdit::textChanged, this, &AdminWidget::onSearch);
}

void AdminWidget::refresh()
{
    loadBooks();
    updateStats();
}

void AdminWidget::loadBooks(const QString& filter)
{
    bookTable_->setColumnCount(7);
    bookTable_->setHorizontalHeaderLabels({ "ID","Title","Author","Dewey","Copies","Status", "Actions"});
    bookTable_->setRowCount(0);

    try {
        BookManager mgr;
        auto books = filter.isEmpty()
            ? mgr.getAllBooks()
            : mgr.searchBooksGui(filter.toStdString());

        for (auto& b : books) {
            int row = bookTable_->rowCount();
            bookTable_->insertRow(row);

            bookTable_->setItem(row, 0, new QTableWidgetItem(QString::number(b.bookId)));
            bookTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.title)));
            bookTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.author)));
            bookTable_->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.dewey)));
            bookTable_->setItem(row, 4, new QTableWidgetItem(QString::number(b.availableCopies) + "/" +
                QString::number(b.totalCopies)));

            auto* statusLbl = new QLabel(b.isAvailable() ? "Available" : "Out");
            statusLbl->setObjectName(b.isAvailable() ? "BadgeAvail" : "BadgeOut");
            statusLbl->setAlignment(Qt::AlignCenter);
            bookTable_->setCellWidget(row, 5, statusLbl);

            auto* editBtn = new QPushButton("Edit");
            editBtn->setObjectName("BtnLink");
            editBtn->setCursor(Qt::PointingHandCursor);

            // Delete button
            auto* deleteBtn = new QPushButton("Delete");
            deleteBtn->setObjectName("BadgeOut");
            deleteBtn->setCursor(Qt::PointingHandCursor);

            auto* actionWidget = new QWidget;
            auto* actionLay = new QHBoxLayout(actionWidget);
            actionLay->setContentsMargins(4, 2, 4, 2);
            actionLay->setSpacing(4);
            actionLay->addWidget(editBtn);
            actionLay->addWidget(deleteBtn);
            actionLay->addStretch();
            bookTable_->setCellWidget(row, 6, actionWidget);

            int capturedRow = row;
            connect(editBtn, &QPushButton::clicked, this, [=] { onEditBook(capturedRow);   });
            connect(deleteBtn, &QPushButton::clicked, this, [=] { onDeleteBook(capturedRow); });

            bookTable_->item(row, 0)->setData(Qt::UserRole, b.bookId);
        }
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Database error", QString::fromStdString(e.what()));
    }
}

void AdminWidget::loadBorrowed()
{
    bookTable_->setColumnCount(6);
    bookTable_->setHorizontalHeaderLabels(
        { "ID","Title","Borrowed by","Borrow date","Due date","Status" });
    bookTable_->setRowCount(0);

    try {
        BorrowManager mgr;
        auto records = mgr.getAllActiveBorrows();
        QString today = QDate::currentDate().toString("yyyy-MM-dd");

        for (auto& r : records) {
            int row = bookTable_->rowCount();
            bookTable_->insertRow(row);

            // BorrowRow has plain public fields, not getter methods
            bookTable_->setItem(row, 0, new QTableWidgetItem(
                QString::number(r.borrowId)));
            bookTable_->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(r.bookTitle)));
            bookTable_->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(r.username)));
            bookTable_->setItem(row, 3, new QTableWidgetItem(
                QString::fromStdString(r.borrowDate).left(10)));
            QString due = QString::fromStdString(r.dueDate).left(10);
            bookTable_->setItem(row, 4, new QTableWidgetItem(due));

            bool overdue = (due < today);
            auto* badge = new QLabel(overdue ? "Overdue" : "On time");
            badge->setObjectName(overdue ? "BadgeOverdue" : "BadgeOntime");
            badge->setAlignment(Qt::AlignCenter);
            bookTable_->setCellWidget(row, 5, badge);
        }
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void AdminWidget::updateStats()
{
    try {
        BookManager mgr;
        BookStats stats = mgr.getStats();

        // BookStats fields: totalBooks, totalCopies, availableCopies, borrowedCopies
        statTotal_->setText(QString::number(stats.totalBooks));
        statAvail_->setText(QString::number(stats.availableCopies));
        statBorrowed_->setText(QString::number(stats.borrowedCopies));

        // Overdue count: query active borrows and count those past due
        BorrowManager bMgr;
        auto records = bMgr.getAllActiveBorrows();
        QString today = QDate::currentDate().toString("yyyy-MM-dd");
        int overdue = 0;
        for (auto& r : records) {
            if (QString::fromStdString(r.dueDate).left(10) < today)
                overdue++;
        }
        statOverdue_->setText(QString::number(overdue));
    }
    catch (...) {}
}

void AdminWidget::onSearch(const QString& text)
{
    loadBooks(text);
}

void AdminWidget::onAddBook()
{
    bool ok;
    QString title = QInputDialog::getText(this, "Add book", "Title:",
        QLineEdit::Normal, "", &ok);
    if (!ok || title.isEmpty()) return;
    QString author = QInputDialog::getText(this, "Add book", "Author:",
        QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString dewey = QInputDialog::getText(this, "Add book", "Dewey decimal:",
        QLineEdit::Normal, "", &ok);
    if (!ok) return;
    int copies = QInputDialog::getInt(this, "Add book", "Number of copies:",
        1, 1, 100, 1, &ok);
    if (!ok) return;

    try {
        BookManager mgr;
        mgr.addBookGui(title.toStdString(), author.toStdString(),
            dewey.toStdString(), copies);
        refresh();
        QMessageBox::information(this, "Success", "Book added successfully.");
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void AdminWidget::onEditBook(int row)
{
    if (row < 0) return;
    int bookId = bookTable_->item(row, 0)->data(Qt::UserRole).toInt();
    try {
        BookManager mgr;
        Book book = mgr.getBookById(bookId);
        bool ok;
        QString title = QInputDialog::getText(this, "Edit book", "Title:",
            QLineEdit::Normal, QString::fromStdString(book.getTitle()), &ok);
        if (!ok) return;
        QString author = QInputDialog::getText(this, "Edit book", "Author:",
            QLineEdit::Normal, QString::fromStdString(book.getAuthor()), &ok);
        if (!ok) return;
        QString dewey = QInputDialog::getText(this, "Edit book", "Dewey:",
            QLineEdit::Normal, QString::fromStdString(book.getDewey()), &ok);
        if (!ok) return;
        int copies = QInputDialog::getInt(this, "Edit book", "Copies:",
            book.getTotalCopies(), 1, 100, 1, &ok);
        if (!ok) return;
        mgr.editBookGui(bookId, title.toStdString(), author.toStdString(),
            dewey.toStdString(), copies);
        refresh();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void AdminWidget::onDeleteBook(int row)
{
    if (row < 0) return;
    int     bookId = bookTable_->item(row, 0)->data(Qt::UserRole).toInt();
    QString title = bookTable_->item(row, 1)->text();
    auto res = QMessageBox::question(this, "Delete book",
        "Delete \"" + title + "\"?",
        QMessageBox::Yes | QMessageBox::No);
    if (res != QMessageBox::Yes) return;
    try {
        BookManager mgr;
        mgr.deleteBook(bookId);
        refresh();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void AdminWidget::switchTab(int index)
{
    if (index == 0) loadBooks();
    else            loadBorrowed();
}