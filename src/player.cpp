#include "player.h"
#include <QtGui>

Player::Player(std::vector<Item> tvVector, bool loadCCTV) : QObject()
{
    playButton = new PlayButton;
    label = new VolumeLabel();
    mainwWindow = new MainWindow(tvVector);

    if(loadCCTV)
        mainwWindow->initCctvs();

    mainwWindow->addVolumeLabel(label->label);

    mainwWindow->addPlayButton(playButton->playButton);
    connect(mainwWindow, SIGNAL(windowResize(QSize )), this, SLOT(resize(QSize )));
    mainwWindow->connect(mainwWindow, &MainWindow::toggleTrigger, this, [=](bool isPlaying){
        //resize(mainwWindow->size());
        label->hide();
        playButton->setChecked(isPlaying);
    });

    connect(playButton, SIGNAL(stateChange(bool)), this, SLOT(stateChanged(bool)));

    connect(mainwWindow, SIGNAL(volumeChanged(int)), label, SLOT(volumeChanged(int)));
}

Player::~Player()
{
    delete mainwWindow;
    delete playButton;
    delete label;
}

void Player::run()
{
    mainwWindow->show();
//    QRect r = mainwWindow->geometry();
//    r.moveCenter(QApplication::desktop()->availableGeometry().center());
//    mainwWindow->setGeometry(r);

}

void Player::resize(QSize size)
{
    playButton->moveTo(size.width()/2 - 64, size.height()/2 - 70);
    label->moveTo(size.width()/2, size.height()/2);
}

void Player::bind(About *about)
{
//    connect(mainwWindow, SIGNAL(menuTrigger(int )), about, SLOT(triggered()));
    connect(mainwWindow, &MainWindow::menuTrigger, about, [=](int item){
        about->show();
    });
}

void Player::stateChanged(bool checked)
{
        qDebug()<<"click triggered occurs"<<checked;
        mainwWindow->play(!checked);
}
