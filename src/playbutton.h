#ifndef PLAYBUTTON_H
#define PLAYBUTTON_H

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class PlayButton : public QObject
{
private:
    QPoint point;
public:
    DImageButton *playButton;

    explicit PlayButton();
    ~PlayButton();
    void moveTo(int x, int y);
    bool isChecked();

public slots:
    void toogle();
};

#endif // PLAYBUTTON_H
