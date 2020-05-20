#include "playbutton.h"
#include <QtGui>

PlayButton::PlayButton() : QObject()
{
    timeId = 0;
    point = QPoint();
    playButton = new QPushButton();
    playButton->setFixedSize(QSize(128, 128));
    playButton->setFocusPolicy(Qt::NoFocus);

    //pauseMap = new QPixmap();
    //pauseMap->load(":/resource/pause.png");
    //playMap = new QPixmap();
    //playMap->load(":/resource/play.png");

//    playButton->setPixmap(*pauseMap);
//    playButton->setAlignment(Qt::AlignCenter);
//    playButton->setAttribute(Qt::WA_TranslucentBackground);
//    playButton->setWindowOpacity(0.8);
    playButton->setStyleSheet("border-image:url(:/resource/play.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 0)");

    connect(playButton, &QPushButton::clicked, this, [=](bool isChecked){
        setChecked(!checked);
        emit stateChange(checked);
    });

    checked = false;
}

PlayButton::~PlayButton()
{
    delete playButton;
    //delete pauseMap;
    //delete playMap;
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

    this->checked = checked;
//    qDebug()<<"click triggered occurs"<<playButton->isChecked();
    if(checked)
    {
        playButton->setStyleSheet("border-image:url(:/resource/play.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0,0)");
    }
    else
    {
        playButton->setStyleSheet("border-image:url(:/resource/pause.png); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0,0)");
    }
}

void PlayButton::show()
{
    if(!playButton->isHidden())
        return;

    playButton->raise();
    playButton->show();
    playButton->move(point);
    if(!checked && timeId == 0)
    {
        timeId = startTimer(5000);
    } else if (checked && timeId > 0) {
        killTimer(timeId);
        timeId = 0;
    }
}

void PlayButton::hide()
{
    playButton->hide();
}

bool PlayButton::isPaused()
{
    return checked;
}

void PlayButton::timerEvent(QTimerEvent *event)
{
   playButton->hide();
   timeId = 0;
}
