#ifndef VOLUMELABEL_H
#define VOLUMELABEL_H

#include <qlabel.h>

class VolumeLabel : public QObject
{
    Q_OBJECT

public:
    QLabel *label;

    VolumeLabel();
    ~VolumeLabel();

private:
    QPoint point;
    int volume;
    int timeout;
    int timerId;

    void timerEvent(QTimerEvent *event);

public:
    void setVolume(int volume);
    int getVolume();
    void moveTo(int x, int y);

public slots:
    void volumeChanged(int volume);
};

#endif // VOLUMELABEL_H
