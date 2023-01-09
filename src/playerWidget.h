#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTimer>
#include "media.h"

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    private:
        QWidget *parent;
        QMenu *contextMenu;
        QLabel * volumeLabel;
        QPushButton * playButton;
        QLabel *m_cover;
        QTimer *volumeTimer;

        int timerId = 0;
        bool pressing;
        qint64 current;
        QPoint mLastMousePosition;
        Media *m_media;

    public:
        explicit PlayerWidget(QWidget *parent=nullptr);
        ~PlayerWidget();
        void loadTv(int groupIndex, int tvIndex);

        void setMedia(Media *media);

    private:
        void showVolumeLabel();
        void showPlayButton();
        void initContextMenu();
        void toggleFullscreen();
        void showPlayFullscreen();
        void showPlayNormal();
        void updateVolume(int volume);

    protected:
        void timerEvent(QTimerEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseDoubleClickEvent( QMouseEvent * e ) override;
        #ifndef QT_NO_WHEELEVENT
        void wheelEvent(QWheelEvent *event) override;
        #endif
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;

        void paintEvent(QPaintEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
    #ifndef QT_NO_CONTEXTMENU
        void contextMenuEvent(QContextMenuEvent *event) override;
    #endif // QT_NO_CONTEXTMENU
        bool event(QEvent* event) override;
        bool eventFilter(QObject *watched, QEvent *event) override;
        // void closeEvent(QCloseEvent *event) override;
        void dragEnterEvent(QDragEnterEvent *event) override;
        void dropEvent(QDropEvent *event) override;

    private slots:
        void printError(const QString &error);
        void updatePlay(Media::State status);
    public slots:
        void setAudioOnly(bool audioOnly=false);
        void switchTv(QAction *action);
        void toogle();
};

#endif //PLAYERWIDGET_H
