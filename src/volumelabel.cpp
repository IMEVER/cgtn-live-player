#include "volumelabel.h"

VolumeLabel::VolumeLabel() : QObject()
{
    timeout = 1500;
    timerId = 0;
    volume = 50;
    point = QPoint();
    label = new QLabel(QString::number(50));
    label->setFixedSize(QSize(140, 100));
    label->setAlignment(Qt::AlignCenter);

    //QFont ft;
    //ft.setPointSize(80);
    //label->setFont(ft);

    //设置颜色
//    QPalette pa;
//    pa.setColor(QPalette::WindowText,Qt::white);
//    label->setPalette(pa);
    //label->setAutoFillBackground(true);
    label->setStyleSheet("border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 120)");
//    pa.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x30));
}

VolumeLabel::~VolumeLabel()
{
    delete label;
}

void VolumeLabel::setVolume(int volume)
{
    this->volume = volume;
}

int VolumeLabel::getVolume()
{
    return volume;
}

void VolumeLabel::moveTo(int x, int y)
{
    point.setX(x - 70);
    point.setY(y - 60);
}

void VolumeLabel::volumeChanged(int volume)
{
    if(this->volume != volume || volume == 100 || volume == 0)
    {
//        qDebug()<<"Volume change to " << volume;
        this->volume = volume;
        label->setText(QString::number(volume));

        label->raise();
        label->show();
        label->move(point);
    }

    if(timerId > 0)
    {
        killTimer(timerId);
        timerId = 0;
    }

    timerId = startTimer(timeout);
}

void VolumeLabel::timerEvent(QTimerEvent *event)
{
    label->hide();
}

void VolumeLabel::hide()
{
    label->hide();
    if(timerId > 0) {
        killTimer(timerId);
        timerId = 0;
    }
}
