#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPoint>
#include <QDateTime>
#include <QStackedLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStackedLayout *mainLayout;
    QMediaPlayer mediaPlayer;

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
    QTimer *poller;
    bool _isPlaying;
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
