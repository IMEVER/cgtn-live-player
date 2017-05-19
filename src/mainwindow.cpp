#include "mainwindow.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <qapplication.h>
#include <QStackedLayout>
#include <QEvent>
#include <vlc/libvlc_media.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    timerId = 0;

    mainLayout = new QStackedLayout();
    mainLayout->setMargin(0);
    mainLayout->setStackingMode(QStackedLayout::StackAll);

    _videoWidget=new QFrame(this);
    mainLayout->addWidget(_videoWidget);

//    QPixmap bkgnd(":/resource/flash.jpg");
//    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);

//    QPalette palette;
//    palette.setBrush(this->backgroundRole(), bkgnd);
//    this->setPalette(palette);

    _vlcinstance=libvlc_new(0, NULL);
    _mp = libvlc_media_player_new (_vlcinstance);
    _m = libvlc_media_new_location( _vlcinstance, url.toLatin1().data());

    libvlc_media_player_set_media (_mp, _m);
    int windid = _videoWidget->winId();
    libvlc_media_player_set_xwindow (_mp, windid );

     QWidget *centralWidget = new QWidget(this);
     centralWidget->setStyleSheet("background-color: black");
     centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    setMinimumSize(QSize(410, 280));

    libvlc_media_player_play(_mp);
    libvlc_video_set_mouse_input(_mp, false);

    resize(_videoWidget->sizeHint());

    initContextMenu();
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::addPlayButton(QPushButton *playButton)
{
    mainLayout->addWidget(playButton);
    playButton->raise();
    playButton->hide();
}

void MainWindow::initContextMenu()
{
    contextMenu = new QMenu(this);
        QMenu *shortMenu = contextMenu->addMenu("ShortCut Key");
        shortMenu->addAction("Up | Down => Change volume");
        shortMenu->addAction("F | DbClick => Toogle fullscreen");
        shortMenu->addAction("F11 => Enter fullscreen");
        shortMenu->addAction("Esc => Exit fullscreen");
        shortMenu->addAction("P | Space | Click => Toogle play and pause");
        shortMenu->addAction("M => Mute");

        contextMenu->addAction("TopHint", this, [=](bool checked){
            toggleTopHint();
        })->setCheckable(true);

        contextMenu->addAction("About", this, [=](){
            emit menuTrigger(1);
        });

        contextMenu->addAction("Exit", qApp, SLOT(quit()));
        contextMenu->setStyleSheet("background-color: grey");
}

void MainWindow::toggleTopHint()
{
        Qt::WindowFlags flags = windowFlags();
        setWindowFlags(flags ^ Qt::WindowStaysOnTopHint ^ Qt::X11BypassWindowManagerHint );
        show();
}

void MainWindow::addVolumeLabel(QLabel *label)
{
    mainLayout->addWidget(label);
    label->raise();
    label->hide();
}

void MainWindow::play(bool play)
{
    //qDebug()<<"toggle play";
    if(libvlc_media_player_is_playing(_mp))
    {
        if(!play)
                libvlc_media_player_pause(_mp);
    }
    else
    {
        if(play)
                libvlc_media_player_play(_mp);
    }
}

MainWindow::~MainWindow()
{
    if(timerId > 0)
        killTimer(timerId);
}

void MainWindow::toogle()
{
    int playing = libvlc_media_player_is_playing(_mp);
    if(playing == 1) {
        libvlc_media_player_pause(_mp);
    } else {
        libvlc_media_player_play(_mp);
    }
    emit toggleTrigger(playing == 1 ? true : false);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_F || keyEvent->key() == Qt::Key_Enter)
        {
            //libvlc_toggle_fullscreen(_mp);
            if(isFullScreen()){
                showNormal();
            } else {
              showFullScreen();
            }
        }
        else if(keyEvent->key() == Qt::Key_F11)
        {
            if(isFullScreen() == false){
                showFullScreen();
            }
        }
        else if(keyEvent->key() == Qt::Key_Escape)
        {
            if(isFullScreen())
            {
                showNormal();
            }
        }
        else if(keyEvent->key() == Qt::Key_Up)
        {
            int volume = libvlc_audio_get_volume(_mp) + 5;
            if(volume > 100)
                volume = 100;
            libvlc_audio_set_volume(_mp, volume);
            emit volumeChanged(volume);
        }
        else if(keyEvent->key() == Qt::Key_Down)
        {
            int volume = libvlc_audio_get_volume(_mp) - 5;
            if(volume < 0)
                volume = 0;
            libvlc_audio_set_volume(_mp, volume);
             emit volumeChanged(volume);
        }
        else if(keyEvent->key() == Qt::Key_M)
        {
            libvlc_audio_toggle_mute(_mp);
        }
        else if(keyEvent->key() == Qt::Key_P || keyEvent->key() == Qt::Key_Space){
            toogle();
        }
        else if(keyEvent->key() == Qt::Key_T)
        {
                toggleTopHint();
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
    if(event->button() == Qt::LeftButton)
    {
    if(timerId == 0)
    {
        //qDebug()<<"click triggered";
        pressing = true;
        timerId = startTimer(700);
        current = QDateTime::currentMSecsSinceEpoch();
    } else {
        killTimer(timerId);
        timerId = 0;
    }
    mLastMousePosition = event->globalPos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons().testFlag(Qt::LeftButton))
    {
    if(timerId > 0)
    {
        killTimer(timerId);
        timerId = 0;
    }


    this->move(this->pos() + (event->globalPos() - mLastMousePosition));
    mLastMousePosition = event->globalPos();
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
        if(now - current < 800)
        {
           toogle();
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
