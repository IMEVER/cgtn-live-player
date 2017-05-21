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
    playButton->setStyleSheet("border-image:url(:/resource/play.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 250)");

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
        playButton->raise();
        playButton->setStyleSheet("border-image:url(:/resource/play.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0,0)");
        playButton->show();
        playButton->move(point);
    }
    else
    {
        playButton->setStyleSheet("border-image:url(:/resource/pause.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0,0)");
        playButton->hide();
    }
}

bool PlayButton::isPaused()
{
    return checked;
}
