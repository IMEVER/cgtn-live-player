#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <QMediaPlayer>
#include <QPoint>
#include <DAboutDialog>
#include <dimagebutton.h>
#include <QDateTime>
#include <QStackedLayout>
#include <dlabel.h>

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStackedLayout *mainLayout;

    void initTitleBar();
    void addPlayButton(DImageButton *playButton);
    void addVolumeLabel(DLabel *label);

    void toggle();
    bool event(QEvent* event);
    void mouseDoubleClickEvent( QMouseEvent * e );
    void resizeEvent(QResizeEvent * event);

private:   
    QMediaPlayer mediaPlayer;
    QString url = QString("https://live.cgtn.com/manifest.m3u8");
    DImageButton *playButton;
    int timerId;
    bool pressing;
    qint64 current;
    void initAboutDialog();
    void toggleTopHint();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void menuTrigger(int menu);
    void toggleTrigger();
    void windowResize(QSize size);
    void volumeChanged(int unit);
};

#endif // MAINWINDOW_H
