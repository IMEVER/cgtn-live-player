#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTimer>

class PlayerWidget : public QWidget
{
    Q_OBJECT

    enum PlayStatus {
        playing,
        stopped,
        loading
    };

    private:
        QWidget *parent;
        QMenu *contextMenu;
        QLabel * volumeLabel;
        QPushButton * playButton;
        QMediaPlayer *mediaPlayer;
        QTimer *volumeTimer;
        qreal ratio;

        int timerId = 0;
        bool pressing;
        qint64 current;
        QPoint mLastMousePosition;

    public:
        explicit PlayerWidget(bool sideShow, QWidget *parent=nullptr);
        ~PlayerWidget();
        void loadTv(int groupIndex, int tvIndex);

        void setSideActionMenuChecked(bool checked);
        void setMediaUrl(QUrl url);
        QUrl getMediaUrl();

        void mouseDoubleClickEvent( QMouseEvent * e ) override;
        #ifndef QT_NO_WHEELEVENT
        void wheelEvent(QWheelEvent *event) override;
        #endif

        void leaveEvent(QEvent *event) override;

    private:
        void toogle();
        void showVolumeLabel();
        void showPlayButton();
        void initContextMenu(bool sideShow);
        void toggleFullscreen();
        void showPlayFullscreen();
        void showPlayNormal();
        void updateVolume(int volume);
        PlayStatus getPlayStatus();
        void updatePlay(PlayStatus status, int percent=0);

        void timerEvent(QTimerEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    protected:
        void resizeEvent(QResizeEvent *event) override;
    #ifndef QT_NO_CONTEXTMENU
        void contextMenuEvent(QContextMenuEvent *event) override;
    #endif // QT_NO_CONTEXTMENU
        bool event(QEvent* event) override;
        // void closeEvent(QCloseEvent *event) override;
        void dragEnterEvent(QDragEnterEvent *event) override;
        void dropEvent(QDropEvent *event) override;

    signals:
        void menuTrigger(int contextMenu);
        void toggleFilterWidget();

    private slots:
        void printError(QMediaPlayer::Error error);
    public slots:
        void switchTv(QAction *action);
};

#endif //PLAYERWIDGET_H