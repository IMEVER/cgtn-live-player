#include "player.h"
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>
#include <singleapplication.h>
#include "listTv.h"
#include "about.h"
#include "playerWidget.h"

Player::Player() : QObject()
{
    mainWindow = new MainWindow();
}

Player::~Player()
{
    mainWindow->deleteLater();
}

void Player::run()
{
    mainWindow->move(QGuiApplication::primaryScreen()->availableGeometry().center() - mainWindow->rect().center());
    mainWindow->show();
    mainWindow->play();
}

void Player::bind()
{
    connect(mainWindow, &MainWindow::openWindowTrigger, [=](int item){
        if(item == 1)
        {
            About *about = new About();
            about->show();
        }
        else if(item == 2)
        {
            ListTvWindow *listWindow = new ListTvWindow(mainWindow);
            listWindow->show();
            connect(listWindow, &ListTvWindow::groupNameChanged, mainWindow, &MainWindow::updateGroupName);
            connect(listWindow, &ListTvWindow::tvTitleChanged, mainWindow, &MainWindow::updateTvTitle);
        }
    });
}
