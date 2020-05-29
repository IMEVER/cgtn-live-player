#ifndef PLAYER_H
#define PLAYER_H

#include "mainwindow.h"

class Player : QObject
{
    Q_OBJECT
public:
    MainWindow *mainWindow;

public:
     Player();
    ~Player();
    void run();
    void addWidget(QWidget *widget);
    void bind();
};

#endif // PLAYER_H