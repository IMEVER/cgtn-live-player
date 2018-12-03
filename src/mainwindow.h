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
    bool event(QEvent* event) override;
    void mouseDoubleClickEvent( QMouseEvent * e ) override;
    void resizeEvent(QResizeEvent * event) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) override;
#endif

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
public slots:
    void toFront();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU:w
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

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
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
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
