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
#include <vector>
#include "item.h"
#include "playbutton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::vector<Item> tvVector, QWidget *parent = 0);
    ~MainWindow();

    QStackedLayout *mainLayout;
    QMediaPlayer * mediaPlayer;

    void initContextMenu();
    void addPlayButton(PlayButton *playButton);
    void addVolumeLabel(QLabel *label);

    void play(bool play);
    bool event(QEvent* event);
    void mouseDoubleClickEvent( QMouseEvent * e );
    void resizeEvent(QResizeEvent * event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
public slots:
    void toFront();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU:w
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:    
    QMenu *contextMenu;
    PlayButton *playBtn;
    QTimer *poller;
    bool _isPlaying;

//    QString url = QString("https://live.cgtn.com/manifest.m3u8");
    std::vector<Item> urls;
    int timerId;
    bool pressing;
    qint64 current;
    QPoint mLastMousePosition;
    qreal ratio;

    bool isPlaying();
    void toogle();
    void toggleTopHint();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void loadTv(Item url);

signals:
    void menuTrigger(int contextMenu);
    void toggleTrigger(bool isPlaying);
    void windowResize(QSize size);
    void volumeChanged(int unit);

private slots:
    void printError(QMediaPlayer::Error error);
    void switchTv(QAction *action);
};

#endif // MAINWINDOW_H
