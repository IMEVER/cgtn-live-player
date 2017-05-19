#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QDateTime>
#include <QStackedLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStackedLayout *mainLayout;

    void initContextMenu();
    void addPlayButton(QPushButton *playButton);
    void addVolumeLabel(QLabel *label);

    void play(bool play);
    bool event(QEvent* event);
    void mouseDoubleClickEvent( QMouseEvent * e );
    void resizeEvent(QResizeEvent * event);

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU:w


private:    
    QMenu *contextMenu;
    QFrame *_videoWidget;
    QTimer *poller;
    bool _isPlaying;
    libvlc_instance_t *_vlcinstance;
    libvlc_media_player_t *_mp;
    libvlc_media_t *_m;
    QString url = QString("https://live.cgtn.com/manifest.m3u8");
    int timerId;
    bool pressing;
    qint64 current;
    QPoint mLastMousePosition;

    void toogle();
    void toggleTopHint();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void menuTrigger(int contextMenu);
    void toggleTrigger(bool isPlaying);
    void windowResize(QSize size);
    void volumeChanged(int unit);
};

#endif // MAINWINDOW_H
