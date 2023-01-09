#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QTimer>

#include "playerWidget.h"
#include "filterWidget.h"
#include "media.h"

class PlayerAdaptor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void play();
    void raiseApp();
    void pause();
    QString playbackStatus();
    QVariantMap metaData();
    QAction *getAction(QString title);
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void resizeVideo();
    void toggleTopHint();
    void initMenubar();

signals:
    void openWindowTrigger(int id);

public slots:
    void toFront();
    void updateGroupName(int index);
    void updateTvTitle(int groupIndex, int tvIndex);

private:
    FilterWidget *filterWidget;
    PlayerWidget *_playerWidget;
    QTimer *m_timer;
    bool keepWidth;
    PlayerAdaptor *m_playerAdaptor;
    Media *media;
};

#endif // MAINWINDOW_H
