#include "player.h"
#include <dutility.h>

DWIDGET_USE_NAMESPACE

Player::Player() : QObject()
{
    playButton = new PlayButton;
    label = new VolumeLabel();
    mainwWindow = new MainWindow;
    mainwWindow->initTitleBar();
    mainwWindow->addVolumeLabel(label->label);

    mainwWindow->addPlayButton(playButton->playButton);//playButton->playButton->show();
    connect(mainwWindow, SIGNAL(windowResize(QSize )), this, SLOT(resize(QSize )));
    mainwWindow->connect(mainwWindow, &MainWindow::toggleTrigger, this, [=](){
        resize(mainwWindow->size());
        playButton->toogle();
    });
    connect(playButton->playButton, SIGNAL(stateChanged()), this, SLOT(stateChanged()));

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
    DUtility::moveToCenter(mainwWindow);
    mainwWindow->show();
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

void Player::stateChanged()
{
    if(playButton->playButton->getState() == DImageButton::Checked || playButton->playButton->getState() == DImageButton::Normal)
    {
        qDebug()<<"click triggered occurs"<<playButton->isChecked();
        mainwWindow->toggle();
        if(playButton->playButton->getState() == DImageButton::Normal)
            playButton->playButton->hide();
    }
}
