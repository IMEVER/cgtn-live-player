#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include "playerWidget.h"
#include "filterWidget.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void play();
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void toFront();

private:
    FilterWidget *filterWidget;
    PlayerWidget *_playerWidget;

    void toggleTopHint();
    void initMenubar();

    protected:
         bool event(QEvent* event) override;

signals:
    void windowResize(QSize size);
    void openWindowTrigger(int id);

public slots:
    void updateGroupName(int index);
    void updateTvTitle(int groupIndex, int tvIndex);
};

#endif // MAINWINDOW_H
