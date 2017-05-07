#include "playbutton.h"

PlayButton::PlayButton() : QObject()
{
    point = QPoint();
    playButton = new DImageButton();
    playButton->setFixedSize(QSize(128, 128));
    playButton->setNormalPic(":/resource/pause.png");
    playButton->setCheckedPic(":/resource/play.png");
    playButton->setAlignment(Qt::AlignCenter);
    playButton->setCheckable(true);
    playButton->setChecked(false);
}

PlayButton::~PlayButton()
{
    delete playButton;
}

void PlayButton::moveTo(int x, int y)
{
    point.setX(x);
    point.setY(y);
    playButton->move(point);
}

void PlayButton::toogle()
{
//    qDebug()<<"click triggered occurs"<<playButton->isChecked();
    playButton->setChecked(!playButton->isChecked());
//    qDebug()<<"click triggered occurs"<<playButton->isChecked();
    if(isChecked())
    {
        playButton->show();
        playButton->move(point);
    }
    else
        playButton->hide();
}

bool PlayButton::isChecked()
{
    return playButton->isChecked();
}
