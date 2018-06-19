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
#include <QNetworkReply>
#include "item.h"

struct Cctv {
    QString title;
    QString code;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::vector<Item> tvVector, QWidget *parent = 0);
    ~MainWindow();

    QStackedLayout *mainLayout;
    QMediaPlayer * mediaPlayer;

    void initContextMenu();
    void addPlayButton(QPushButton *playButton);
    void addVolumeLabel(QLabel *label);

    void play(bool play);
    bool event(QEvent* event);
    void mouseDoubleClickEvent( QMouseEvent * e );
    void resizeEvent(QResizeEvent * event);
    void initCctvs();

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
    QTimer *poller;
    bool _isPlaying;

//    QString url = QString("https://live.cgtn.com/manifest.m3u8");
    std::vector<Item> urls;
    Cctv cctvs[18] = {{QString("Cctv1 综合"), QString("cctv1")}, {QString("Cctv2 财经"), QString("cctv2")}, {QString("Cctv3 综艺"), QString("cctv3")}, {QString("Cctv4 亚洲"), QString("cctv4")}, {QString("Cctv4 欧洲"), QString("cctveurope")}, {QString("Cctv4 美洲"), QString("cctvamerica")}, {QString("Cctv5 体育"), QString("cctv5")}, {QString("Cctv6 电影"), QString("cctv6")}, {QString("Cctv7 军事农业"), QString("cctv7")}, {QString("Cctv8 电视剧"), QString("cctv8")}, {QString("Cctv9 纪录"), QString("cctv9")}, {QString("Cctv10 科教"), QString("cctv10")}, {QString("Cctv11 戏曲"), QString("cctv11")}, {QString("Cctv12 社会与法制"), QString("cctv12")}, {QString("Cctv13 新闻"), QString("cctv13")}, {QString("Cctv14 少儿"), QString("cctvchild")}, {QString("Cctv15 音乐"), QString("cctv15")}, {QString("Cctv5+ 体育赛事"), QString("cctv5plus")}};
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
    void requestReceived(QNetworkReply *reply);
};

#endif // MAINWINDOW_H
