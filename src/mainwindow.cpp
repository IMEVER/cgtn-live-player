#include "mainwindow.h"
#include <QVideoWidget>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <qapplication.h>
#include <QStackedLayout>
#include <QEvent>
#include <time.h>
#include <QStandardPaths>
#include <QFile>
#include "logger.h"
#include <QClipboard>

MainWindow::MainWindow(std::vector<Item> tvVector, QWidget *parent) : QMainWindow(parent)
{
    timerId = 0;
    this->urls = tvVector;

    mainLayout = new QStackedLayout();
    mainLayout->setMargin(0);
    mainLayout->setStackingMode(QStackedLayout::StackAll);

//    QPixmap bkgnd(":/resource/flash.jpg");
//    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
//    QPalette palette;
//    palette.setBrush(this->backgroundRole(), bkgnd);
//    this->setPalette(palette);

     QVideoWidget *_videoWidget=new QVideoWidget;
     _videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
     mainLayout->addWidget(_videoWidget);

     QWidget *centralWidget = new QWidget(this);
     centralWidget->setStyleSheet("background-color: black");
     centralWidget->setLayout(mainLayout);
     setCentralWidget(centralWidget);

     //videoWidget->setSource(&mediaPlayer);

     mediaPlayer = new QMediaPlayer(0, (QMediaPlayer::StreamPlayback));
     mediaPlayer->setPlaybackRate(1);
     mediaPlayer->setVideoOutput(_videoWidget);
     //mediaPlayer->setMedia(QUrl(urls[0]->getUrl()));
     mediaPlayer->setVolume(50);

     ratio = (qreal)404.7/720;

     connect(mediaPlayer, static_cast<void(QMediaObject::*)(const QString &, const QVariant &)>(&QMediaObject::metaDataChanged),
         [=](const QString &key, const QVariant &value){
         if(key == "Resolution")
         {
             QSize size = value.toSize();
             //resize(size.width(), size.height());
             Logger::instance().log("Width: " + std::to_string(size.width()) + ", Height: " + std::to_string(size.height()));
             mediaPlayer->play();
         }
      });
     QObject::connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(printError(QMediaPlayer::Error)));

    setMinimumSize(QSize(410, 280));
    resize(720, 405);
    initContextMenu();

    loadTv(urls[0]);
}

void MainWindow::initCctvs()
{    QString authUrl = QString("http://vdn.live.cntv.cn/api2/liveHtml5.do?channel=pa://cctv_p2p_hd%1&client=html5&tsp=%2&_code_=%3");
     time_t timestamp = time(NULL);
     for(int i=0; i<18;i++)
     {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));

        Cctv cctv = cctvs[i];
        Logger::instance().log("CCTV auth: " + authUrl.arg(cctv.code).arg(timestamp).arg(cctv.code).toStdString());
        QNetworkRequest request = QNetworkRequest(QUrl(authUrl.arg(cctv.code).arg(timestamp).arg(cctv.code)));

        manager->get(request);
    }
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    Logger::instance().log("Right click trigger", Logger::kLogLevelInfo);
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

        QMenu *tvMenu = contextMenu->addMenu("TV");
        QActionGroup *groups = new QActionGroup(this);

        for(int i=0, len=urls.size(); i < len; i++) {
            Item url = urls[i];

            QAction *action = new QAction(url.getTitle(), this);
            action->setData(i);
            action->setCheckable(true);
            if(i == 0)
                action->setChecked(true);
            groups->addAction(action);
            tvMenu->addAction(action);
            if(url.isNeedSeparate())
                tvMenu->addSeparator();
        }
        connect(tvMenu, SIGNAL(triggered(QAction *)), this, SLOT(switchTv(QAction *)));

        contextMenu->addAction("Copy url", this, [=] () {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(mediaPlayer->media().canonicalUrl().url());
//            QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
        });

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
    Logger::instance().log("Media player state: " + mediaPlayer->state(), Logger::kLogLevelInfo);
    return mediaPlayer->state() == QMediaPlayer::PlayingState;
}

void MainWindow::switchTv(QAction *action)
{
    Item url = urls[action->data().toInt()];
    loadTv(url);
}

void MainWindow::loadTv(Item url)
{
    mediaPlayer->setMedia(QUrl(url.getUrl()));
    setWindowTitle("Cgtv Player: " + url.getTitle());
    Logger::instance().log("Switch to tv " + url.getTitle().toStdString(), Logger::kLogLevelInfo);
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
    Logger::instance().log("toggle play");
    if(isPlaying())
    {
        if(!play)
                mediaPlayer->pause();
    }
    else
    {
        if(play)
                mediaPlayer->play();
    }
}

MainWindow::~MainWindow()
{
    if(timerId > 0)
        killTimer(timerId);
}

void MainWindow::toogle()
{
    Logger::instance().log("toggle play");
    bool playing = isPlaying();
    if(playing)
    {
        mediaPlayer->pause();
    }
    else
    {
        mediaPlayer->play();
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
                   int volume = mediaPlayer->volume();
                  if(volume < 100) {
                     volume += 5;
                      if(volume > 100)
                         volume = 100;
                     mediaPlayer->setVolume(volume);
                 }
                emit volumeChanged(volume);
            }
        }
        else if(keyEvent->key() == Qt::Key_Down)
        {
            if(isPlaying()) {
                int volume = mediaPlayer->volume();
                if(volume > 0) {
                    volume -= 5;
                    if(volume < 0)
                        volume = 0;
                    mediaPlayer->setVolume(volume);
                }
                emit volumeChanged(volume);
            }
        }
        else if(keyEvent->key() == Qt::Key_M)
        {
            mediaPlayer->setMuted(!mediaPlayer->isMuted());
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
    QSize oldSize = event->oldSize();
    QSize newSize = event->size();
    QSize fixedSize(windowSize.width(), windowSize.height());

//    if(newSize.width() != oldSize.width())
//    {
//        fixedSize.setWidth(newSize.width());
//        fixedSize.setHeight((int)(newSize.width() * ratio));
//        qDebug()<<"Resize to width: "<<fixedSize.width()<<", height: "<<fixedSize.height()<<", by ratio: "<<ratio<<", "<<(int)(newSize.width() * ratio);
//        resize(fixedSize);
//    }

    emit windowResize(fixedSize);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
    if(timerId == 0)
    {    
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

void MainWindow::requestReceived(QNetworkReply *reply)
{
    reply->deleteLater();

    if(reply->error() == QNetworkReply::NoError) {
        // Get the http status code
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300) // Success
        {
             // Here we got the final reply
            QString replyText = reply->readAll();
            int i1 = replyText.indexOf("'");
            int i2 = replyText.indexOf("'", i1 + 1);
            Logger::instance().log(replyText.mid(replyText.indexOf("'") + 1, i2-i1 - 1).toStdString());

            QString cloudName = replyText.contains("LIVE-HLS-CDN-ALI") ? "阿里云" : (replyText.contains("LIVE-HLS-CDN-QQ") ? "腾讯云" : "其他");

             QRegExp rx("\"hls1\":\"([^\"]+)\"");
            int pos = replyText.indexOf(rx);
            if(pos >= 0)
            {
                QString url = rx.cap(1);
                QString authUrl = reply->request().url().url();
                for(int i=0; i<18;i++)
                {
                    Cctv cctv = cctvs[i];
                    if(authUrl.endsWith("&_code_=" + cctv.code))
                    {
                        Logger::instance().log("code: " + cctv.title.toStdString() + "\turl: " + url.toStdString());
                        urls.push_back(Item(cctv.title, url));
                        QAction *tvMenu = contextMenu->actions().at(1);
                        QAction *action = new QAction(cctv.title + " (" + cloudName + ")", this);
                        int last = urls.size() - 1;
                        action->setData(last);
                        action->setCheckable(true);
                        tvMenu->menu()->actions().at(0)->actionGroup()->addAction(action);
                        QList<QAction *> actions = tvMenu->menu()->actions();
                        int index=actions.size() - 1;
                        while (index > 0 && !actions.at(index)->isSeparator() && actions.at(index)->text().replace(" ", "").compare(cctv.title.replace(" ", "")) > 0) {
                            index--;
                        }
                        if(index == actions.size() -1)
                            tvMenu->menu()->addAction(action);
                        else
                            tvMenu->menu()->insertAction(actions.at(index + 1), action);
                        break;
                    }
                }
            } else {
                Logger::instance().log("Cannot found url");
            }
        }
        else if (v >= 300 && v < 400) // Redirection
        {
            // Get the redirection url
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            // Because the redirection url can be relative,
            // we have to use the previous one to resolve it
            newUrl = reply->url().resolved(newUrl);

            QNetworkAccessManager *manager = reply->manager();
            QNetworkRequest redirection(newUrl);
            QNetworkReply *newReply = manager->get(redirection);

            return; // to keep the manager for the next request
        }
    }
    else
    {
        // Error
        Logger::instance().log(reply->errorString().toStdString());
    }

    reply->manager()->deleteLater();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QClipboard *clipboard = QApplication::clipboard();
    QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    event->accept();
}

void MainWindow::printError(QMediaPlayer::Error error)
{
    QString str;
    QDebug(&str)<<error;
    Logger::instance().log("Player error: " + str.toStdString(), Logger::kLogLevelError);
    QDebug(&str)<<mediaPlayer->state();
    Logger::instance().log("Player status: " + str.toStdString());
    QDebug(&str)<<mediaPlayer->mediaStatus();
    Logger::instance().log("Player MediaStatus: " + str.toStdString());
    mediaPlayer->play();
}
