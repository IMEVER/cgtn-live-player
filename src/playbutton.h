#ifndef PLAYBUTTON_H
#define PLAYBUTTON_H

#include <QPushButton>

class PlayButton : public QObject
{
    Q_OBJECT

private:
    QPoint point;
    bool checked;
    //QPixmap *pauseMap;
    //QPixmap *playMap;
    int timeId;

public:
    PlayButton();
    ~PlayButton();
    QPushButton *playButton;
    void moveTo(int x, int y);
    bool isPaused();
    void show();
    void hide();

private:
    void timerEvent(QTimerEvent *event);

signals:
    void stateChange(bool checked = false);

public slots:
    void setChecked(bool checked);
};

#endif // PLAYBUTTON_H
