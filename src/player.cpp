#include "player.h"
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>
#include <singleapplication.h>
#include "listTv.h"
#include "about.h"

Player::Player() : QObject()
{
    mainWindow = new MainWindow();
}

Player::~Player()
{
    delete mainWindow;
}

void Player::run()
{
    mainWindow->move(QApplication::desktop()->screen()->rect().center() - mainWindow->rect().center());
    mainWindow->show();
    mainWindow->loadTv();
}

void Player::bind()
{
//    connect(mainwWindow, SIGNAL(menuTrigger(int )), about, SLOT(triggered()));
    connect(mainWindow, &MainWindow::menuTrigger, [=](int item){
        if(item == 1)
        {
            About *about = new About;
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