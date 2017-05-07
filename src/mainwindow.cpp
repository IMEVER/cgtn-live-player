#include "mainwindow.h"
#include <QVideoWidget>
#include <QMediaPlaylist>
#include <dvideowidget.h>
#include <QKeyEvent>
#include <DTitlebar>
#include <QVBoxLayout>
#include <dthememanager.h>
#include <QApplication>
#include <qapplication.h>
#include <DAboutDialog>
#include <QStackedLayout>
#include <QEvent>

using namespace Dtk::Widget;

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent)
{
    timerId = 0;
    DThemeManager *themeManager = DThemeManager::instance();
    themeManager->setTheme("dark");

    mainLayout = new QStackedLayout();
    mainLayout->setMargin(0);
    mainLayout->setStackingMode(QStackedLayout::StackAll);

    DVideoWidget *videoWidget = new DVideoWidget;

    mainLayout->addWidget(videoWidget);

     QWidget *centralWidget = new QWidget(this);
     centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    setMinimumSize(QSize(400, 400));

    resize(722, 578);


    QPalette* palette = new QPalette();

    palette->setBrush(QPalette::Background, Qt::black);
    videoWidget->setPalette(*palette);
    videoWidget->setAutoFillBackground(true);

    QImage _images;
    _images.load(":/resource/flash.jpg");

    //QBrush brush = QBrush(QPixmap(":/resource/flash.jpg"));

    QBrush brush = QBrush(QPixmap(":/resource/flash.jpg"));

    palette->setBrush(QPalette::Window,
    QBrush(_images.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
    videoWidget->setPalette(*palette);

    delete palette;

    videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    videoWidget->setSource(&mediaPlayer);

    mediaPlayer.setMedia(QUrl(url));

    mediaPlayer.setVolume(50);

    connect(&mediaPlayer, static_cast<void(QMediaObject::*)(const QString &, const QVariant &)>(&QMediaObject::metaDataChanged),
        [=](const QString &key, const QVariant &value){
        if(key == "Resolution")
        {
            QSize size = value.toSize();
            resize(size.width(), size.height());
            mediaPlayer.play();
        }
    });
}

void MainWindow::addPlayButton(DImageButton *playButton)
{
    mainLayout->addWidget(playButton);
    playButton->raise();
    playButton->hide();
}

void MainWindow::initTitleBar()
{
    DTitlebar *titlebar = this->titleBar();

    if (titlebar) {
        titlebar->setTitle("Cgtn live player");
        titlebar->setMenu(new QMenu(titlebar));
        titlebar->setSeparatorVisible(true);

        QMenu *shortMenu = titlebar->menu()->addMenu("ShortCut Key");
        shortMenu->addAction("Up | Down => Change volume");
        shortMenu->addAction("F | DbClick => Toogle fullscreen");
        shortMenu->addAction("F11 => Enter fullscreen");
        shortMenu->addAction("Esc => Exit fullscreen");
        shortMenu->addAction("P | Space | Click => Toogle play and pause");
        shortMenu->addAction("M => Mute");

        QMenu *menu = titlebar->menu()->addMenu("Style");

        menu->addAction("dark", this, [=](){
            DThemeManager *themeManager = DThemeManager::instance();
            themeManager->setTheme("dark");
            titlebar->menu()->actions().at(1)->menu()->actions().at(1)->setChecked(false);
        })->setCheckable(true);
        titlebar->menu()->actions().at(1)->menu()->actions().at(0)->setChecked(true);

        menu->addAction("light", this, [=](){
            DThemeManager *themeManager = DThemeManager::instance();
            themeManager->setTheme("light");
            titlebar->menu()->actions().at(1)->menu()->actions().at(0)->setChecked(false);
        })->setCheckable(true);

        titlebar->menu()->addAction("TopHint", this, [=](bool checked){
             Qt::WindowFlags flags = windowFlags();
             setWindowFlags(flags ^ Qt::WindowStaysOnTopHint);
             show();
        })->setCheckable(true);

        titlebar->menu()->addAction("About", this, [=](){
            emit menuTrigger(1);
        });

         titlebar->menu()->addAction("Exit", qApp, SLOT(quit()));
    }
}

void MainWindow::addVolumeLabel(DLabel *label)
{
    mainLayout->addWidget(label);
    label->raise();
    label->hide();
}

void MainWindow::toggle()
{
    qDebug()<<"toggle play";
    if(mediaPlayer.state() == QMediaPlayer::PlayingState)
    {
        mediaPlayer.pause();
    }
    else
    {
        mediaPlayer.play();
    }
}

MainWindow::~MainWindow()
{
    if(timerId > 0)
        killTimer(timerId);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_F || keyEvent->key() == Qt::Key_Enter)
        {
            if(isFullScreen()){
                titleBar()->show();
                showNormal();
            } else {
                titleBar()->hide();
              showFullScreen();
            }
        }
        else if(keyEvent->key() == Qt::Key_F11)
        {
            if(isFullScreen() == false){
                titleBar()->hide();
                showFullScreen();
            }
        }
        else if(keyEvent->key() == Qt::Key_Escape)
        {
            if(isFullScreen())
            {
                titleBar()->show();
                showNormal();
            }
        }
        else if(keyEvent->key() == Qt::Key_Up)
        {
            int volume = mediaPlayer.volume() + 5;
            if(volume > 100)
                volume = 100;
            mediaPlayer.setVolume(volume);
            emit volumeChanged(volume);
        }
        else if(keyEvent->key() == Qt::Key_Down)
        {
            int volume = mediaPlayer.volume() - 5;
            if(volume < 0)
                volume = 0;
             mediaPlayer.setVolume(volume > 4 ? volume-5 : 0);
             emit volumeChanged(volume);
        }
        else if(keyEvent->key() == Qt::Key_M)
        {
            mediaPlayer.setMuted(!mediaPlayer.isMuted());
        }
        else if(keyEvent->key() == Qt::Key_P || keyEvent->key() == Qt::Key_Space){
            emit toggleTrigger();
        }
        else if(keyEvent->key() == Qt::Key_T)
        {
            Qt::WindowFlags flags = windowFlags();
            setWindowFlags(flags ^ Qt::WindowStaysOnTopHint);
            show();
        }
    }
    return QMainWindow::event(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize windowSize = size();

    emit windowResize(windowSize);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(timerId == 0)
    {
        qDebug()<<"click triggered";
        pressing = true;
        timerId = startTimer(700);
        current = QDateTime::currentMSecsSinceEpoch();
    } else {
        killTimer(timerId);
        timerId = 0;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(timerId > 0)
    {
        killTimer(timerId);
        timerId = 0;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    pressing = false;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(timerId > 0 && !pressing)
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        killTimer(timerId);
        timerId = 0;
        if(now - current < 700)
        {
            emit toggleTrigger();
        }
    }
}

void MainWindow::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        if(timerId > 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
        if(isFullScreen()){
            showNormal();
        } else {
          showFullScreen();
        }
    }
}
