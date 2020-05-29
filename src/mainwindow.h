#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPoint>
#include <QDateTime>
#include <QMenuBar>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QVideoWidget>
#include <QLineEdit>
#include <QStandardItemModel>
class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum PlayStatus {
        playing,
        stopped,
        loading
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMediaPlayer * mediaPlayer;

    void initContextMenu();
    void loadTv();
    void loadTv(int groupIndex, int tvIndex);

    void play(PlayStatus status);
    bool event(QEvent* event) override;
    void mouseDoubleClickEvent( QMouseEvent * e ) override;
    void resizeEvent(QResizeEvent * event) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) override;
#endif

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

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
    QStandardItemModel *model;
    QLineEdit * search;
    QWidget *filterWidget;
    QVideoWidget *_videoWidget;
    QMenu *contextMenu;
    QLabel * volumeLabel;
    QPushButton * playButton;
    QTreeView *treeMenu;
    QTimer *poller;
    PlayStatus playStatus;
    QSortFilterProxyModel *filterModel;

//    QString url = QString("https://live.cgtn.com/manifest.m3u8");
    int timerId;
    int volumeTimerId;
    int percent;
    bool pressing;
    qint64 current;
    QPoint mLastMousePosition;
    qreal ratio;

    void toogle();
    void showVolumeLabel(int volume);
    void showPlayButton();
    void toggleTopHint();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

signals:
    void menuTrigger(int contextMenu);
    void toggleTrigger(PlayStatus);
    void windowResize(QSize size);
    void volumeChanged(int unit);

private slots:
    void printError(QMediaPlayer::Error error);
    void switchTv(QAction *action);

public slots:
    void updateGroupName(int index);
    void updateTvTitle(int groupIndex, int tvIndex);
};

#endif // MAINWINDOW_H
