#include "mainwindow.h"
#include <QVideoWidget>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <qapplication.h>
#include <QStackedLayout>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    timerId = 0;

    mainLayout = new QStackedLayout();
    mainLayout->setMargin(0);
    mainLayout->setStackingMode(QStackedLayout::StackAll);

//    QPixmap bkgnd(":/resource/flash.jpg");
//    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
//    QPalette palette;
//    palette.setBrush(this->backgroundRole(), bkgnd);
//    this->setPalette(palette);

     QVideoWidget *_videoWidget=new QVideoWidget;
     _videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
     mainLayout->addWidget(_videoWidget);

     QWidget *centralWidget = new QWidget(this);
     centralWidget->setStyleSheet("background-color: black");
     centralWidget->setLayout(mainLayout);
     setCentralWidget(centralWidget);

     //videoWidget->setSource(&mediaPlayer);

     mediaPlayer.setVideoOutput(_videoWidget);
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

    setMinimumSize(QSize(410, 280));
    resize(_videoWidget->sizeHint());
    initContextMenu();
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug()<<"Right click trigger";
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
        contextMenu->setStyleSheet("border-radius: 5px; background-color: grey");
}

bool MainWindow::isPlaying()
{
    return mediaPlayer.state() == QMediaPlayer::PlayingState;
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
    qDebug()<<"toggle play";
    if(isPlaying())
    {
        if(!play)
                mediaPlayer.pause();
    }
    else
    {
        if(play)
                mediaPlayer.play();
    }
}

MainWindow::~MainWindow()
{
    if(timerId > 0)
        killTimer(timerId);
}

void MainWindow::toogle()
{
    qDebug()<<"toggle play";
    bool playing = isPlaying();
    if(playing)
    {
        mediaPlayer.pause();
    }
    else
    {
        mediaPlayer.play();
    }

    emit toggleTrigger(playing);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_F || keyEvent->key() == Qt::Key_Enter)
        {
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
            if(isPlaying()) {
                   int volume = mediaPlayer.volume();
                  if(volume < 100) {
                     volume += 5;
                      if(volume > 100)
                         volume = 100;
                     mediaPlayer.setVolume(volume);
                 }
                emit volumeChanged(volume);
            }
        }
        else if(keyEvent->key() == Qt::Key_Down)
        {
            if(isPlaying()) {
                int volume = mediaPlayer.volume();
                if(volume > 0) {
                    volume -= 5;
                    if(volume < 0)
                        volume = 0;
                    mediaPlayer.setVolume(volume);
                }
                emit volumeChanged(volume);
            }
        }
        else if(keyEvent->key() == Qt::Key_M)
        {
            mediaPlayer.setMuted(!mediaPlayer.isMuted());
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
