#include "main_window.h"
#include "login_widget.h"
#include "admin_widget.h"
#include "user_widget.h"
#include "opac_style.h"
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("OPAC Library System");
    setMinimumSize(900, 620);
    resize(1100, 680);

    // Apply global stylesheet
    qApp->setStyleSheet(OpacStyle::sheet());

    stack_ = new QStackedWidget(this);
    setCentralWidget(stack_);

    loginWidget_ = new LoginWidget(this);
    stack_->addWidget(loginWidget_);  // index 0 = login

    connect(loginWidget_, &LoginWidget::loginAsAdmin, this, &MainWindow::showAdmin);
    connect(loginWidget_, &LoginWidget::loginAsUser, this, &MainWindow::showUser);

    stack_->setCurrentIndex(0);
}

void MainWindow::showAdmin(const QString& username)
{
    if (adminWidget_) {
        stack_->removeWidget(adminWidget_);
        delete adminWidget_;
        adminWidget_ = nullptr;
    }

    adminWidget_ = new AdminWidget(username.toStdString(), this);
    connect(adminWidget_, &AdminWidget::logoutRequested, this, &MainWindow::showLogin);
    stack_->addWidget(adminWidget_);
    stack_->setCurrentWidget(adminWidget_);
}

void MainWindow::showUser(const QString& username)
{
    if (userWidget_) {
        stack_->removeWidget(userWidget_);
        delete userWidget_;
        userWidget_ = nullptr;
    }

    userWidget_ = new UserWidget(username.toStdString(), this);
    connect(userWidget_, &UserWidget::logoutRequested, this, &MainWindow::showLogin);
    stack_->addWidget(userWidget_);
    stack_->setCurrentWidget(userWidget_);
}

void MainWindow::showLogin()
{
    stack_->setCurrentIndex(0);
}