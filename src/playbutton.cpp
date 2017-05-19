#include "playbutton.h"
#include <QtGui>

PlayButton::PlayButton() : QObject()
{
    point = QPoint();
    playButton = new QPushButton();
    playButton->setFixedSize(QSize(128, 128));

    pauseMap = new QPixmap();
    pauseMap->load(":/resource/pause.png");
    playMap = new QPixmap();
    playMap->load(":/resource/play.png");

//    playButton->setPixmap(*pauseMap);
//    playButton->setAlignment(Qt::AlignCenter);
    playButton->setStyleSheet("border-image:url(:/resource/play.png);");

    playButton->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    playButton->setAttribute(Qt::WA_TranslucentBackground);

    connect(playButton, &QPushButton::clicked, this, [=](bool isChecked){
        qDebug()<<checked;
        setChecked(!checked);
        qDebug()<<checked;
        emit stateChange(checked);
    });

    checked = false;
}

PlayButton::~PlayButton()
{
    delete playButton;
    delete pauseMap;
    delete playMap;
}

void PlayButton::moveTo(int x, int y)
{
    point.setX(x);
    point.setY(y);
    playButton->move(point);
}

void PlayButton::setChecked(bool checked)
{
//    qDebug()<<"click triggered occurs"<<playButton->isChecked();
    if(this->checked == checked)
        return;

    this->checked = checked;
//    qDebug()<<"click triggered occurs"<<playButton->isChecked();
    if(checked)
    {
//        playButton->setPixmap(*pauseMap);
        playButton->setStyleSheet("border-image:url(:/resource/play.png);");
        playButton->show();
        playButton->move(point);
    }
    else
    {
        playButton->setStyleSheet("border-image:url(:/resource/pause.png);");
//        playButton->setPixmap(*playMap);
        playButton->hide();
    }
}

bool PlayButton::isPaused()
{
    return checked;
}
