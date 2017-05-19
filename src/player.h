#ifndef PLAYER_H
#define PLAYER_H

#include "mainwindow.h"
#include "playbutton.h"
#include "volumelabel.h"
#include "about.h"

class Player : QObject
{
    Q_OBJECT
private:
    PlayButton *playButton;
    VolumeLabel *label;
public:
    MainWindow *mainwWindow;

public:
     Player();
    ~Player();
    void run();
    void addWidget(QWidget *widget);
    void bind(About *about);

public slots:
    void resize(QSize size);
    void stateChanged(bool checked);
};

#endif // PLAYER_H
