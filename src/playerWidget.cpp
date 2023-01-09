#include "playerWidget.h"
#include "mainwindow.h"
#include "logger.h"
#include "channelconf.h"
#include "conf.h"
#include <QVBoxLayout>
#include <QTimerEvent>
#include <QDateTime>
#include <QApplication>
#include <QContextMenuEvent>
#include <QClipboard>
#include <QMimeData>
#include <QVariant>
#include <QStyleOption>
#include <QStyle>

PlayerWidget::PlayerWidget(QWidget *parent): QWidget(parent)
{
    this->parent = parent;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: rgba(28,28,28,255);");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));

    volumeLabel = new QLabel(QString::number(Conf::instance()->volume()), this);
    volumeLabel->setFixedSize(QSize(150, 100));
    volumeLabel->setAlignment(Qt::AlignCenter);
    volumeLabel->setStyleSheet("QLabel {border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0, 0, 0, 200); }");

    playButton = new QPushButton(this);
    playButton->setFixedSize(QSize(128, 128));
    playButton->setFocusPolicy(Qt::NoFocus);
    playButton->setStyleSheet("QPushButton {border-image:url(:/resource/loading.gif); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 100);} ");
    connect(playButton, &QPushButton::clicked, this, &PlayerWidget::toogle);

    m_cover = new QLabel(this);
    m_cover->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 0); }");
    m_cover->installEventFilter(this);
    m_cover->setMouseTracking(true);

    initContextMenu();
    setAcceptDrops(true);
    setMouseTracking(true);

    volumeTimer = new QTimer(this);
    volumeTimer->setSingleShot(true);
    volumeTimer->setInterval(1500);
    connect(volumeTimer, &QTimer::timeout, this, [ this ] {
        volumeLabel->lower();
        playButton->lower();
    });
}

PlayerWidget::~PlayerWidget()
{
    disconnect(m_media);
    if (timerId > 0)
        killTimer(timerId);
}

void PlayerWidget::setMedia(Media *media)
{
    this->m_media = media;

    layout()->addWidget(m_media->videoWidget());
//    m_media->videoWidget()->installEventFilter(this);

    m_media->setVolume(Conf::instance()->volume()/100.0);

    connect(m_media, &Media::sourceChanged, [this] {
        Logger::instance().log("Change to another media source");
        setFocus();
    });
    connect(m_media, &Media::stateChanged, this, &PlayerWidget::updatePlay);

    QObject::connect(m_media, SIGNAL(error(const QString)), this, SLOT(printError(QString)));
    m_cover->raise();
}

#ifndef QT_NO_CONTEXTMENU
void PlayerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Logger::instance().log("Right click trigger", Logger::kLogLevelInfo);

    QClipboard *clipboard = QApplication::clipboard();
    QString url = clipboard->text(QClipboard::Clipboard);
    bool hasUrl = !url.isEmpty() && !url.isNull() && url.startsWith("http");

    contextMenu->actions()[2]->setDisabled(!hasUrl);
    contextMenu->actions().at(3)->setDisabled(m_media->file().isEmpty());

    contextMenu->exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void PlayerWidget::initContextMenu()
{
    contextMenu = new QMenu();
    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("shortcut"));
    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("tv"));

    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("pasteUrl"));

    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("copyUrl"));
    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("sidebar"));

    // contextMenu->addAction("提示", this, [=](bool checked){
    //     toggleTopHint();
    // })->setCheckable(true);

    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("edit"));
    contextMenu->addAction(qobject_cast<MainWindow*>(parent)->getAction("about"));

    contextMenu->setStyleSheet("border-radius: 5px; background-color: grey");
}

void PlayerWidget::updateVolume(int volume)
{
    QVariant v = volumeLabel->property("volume");
    int old = v.isValid() ? v.toInt() : 0;
    if(old != volume)
    {
        volumeLabel->setProperty("volume", volume);
        volumeLabel->setText(QString::number(volume));
        Conf::instance()->setVolume(volume);
    }
    showVolumeLabel();
}

void PlayerWidget::showVolumeLabel()
{
    volumeTimer->stop();

    playButton->lower();
    volumeLabel->raise();
    volumeTimer->start();
}

void PlayerWidget::updatePlay(Media::State status)
{
    QVariant v = playButton->property("status");
    Media::State oldStatus = v.isValid() ? (Media::State)v.toInt() : Media::StoppedState;
    if (oldStatus != status)
    {
        QString img;
        playButton->setProperty("status", status);
        switch (status)
        {
            case Media::PlayingState:
                img = "pause.png";
                break;
            case Media::LoadingState:
            case Media::BufferingState:
                img = "loading.gif";
                break;
            default:
                img = "play.png";
                break;
        }
        playButton->setStyleSheet(QString("QPushButton {background-image:url(:/resource/%1); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 100);} ").arg(img));
    }

    if (status == Media::LoadingState || status == Media::BufferingState)
    {/*
        v = playButton->property("percent");
        int oldPercent = v.isValid() ? v.toInt() : -1;
        if (oldPercent != percent)
        {
            playButton->setProperty("percent", percent);
            playButton->setText(QString::number(percent) + "%");
        }*/
        showPlayButton();
    }
    else if(oldStatus != status)
    {
        playButton->setText("");
        if (status == Media::PlayingState)
        {
            playButton->lower();
            volumeTimer->stop();
        }
        else
        {
            showPlayButton();
        }
    }
}

void PlayerWidget::showPlayButton()
{
    volumeTimer->stop();
    volumeLabel->lower();

    playButton->raise();

    if (m_media->state() == Media::PlayingState)    
        volumeTimer->start();    
}

void PlayerWidget::loadTv(int groupIndex, int tvIndex)
{
    Channel *url = ChannelConf::instance()->getJsonConf().groups->at(groupIndex)->tvs->at(tvIndex);
    m_media->play(url->url);
    parent->setWindowTitle("央视外语频道: " + url->title);
    Logger::instance().log("Switch to tv " + url->title.toStdString() + "\t" + url->url.toStdString(), Logger::kLogLevelInfo);
}

void PlayerWidget::switchTv(QAction *action)
{
    QVector<int> ss = action->data().value<QVector<int>>();
    loadTv(ss[0], ss[1]);
}

void PlayerWidget::setAudioOnly(bool audioOnly)
{
    bool playing = m_media->state() == Media::PlayingState || m_media->state() == Media::PausedState || m_media->state() == Media::BufferingState;
    QString url = m_media->file();
    if(playing && !audioOnly)
        m_media->stop();

    QTimer::singleShot(10, [this, playing, audioOnly, url]{
        m_media->setAudioOnly(audioOnly);

        if(playing && !audioOnly)
            QTimer::singleShot(10, [this, url]{ m_media->play(url);});
    });
}

void PlayerWidget::toogle()
{
    Logger::instance().log("toggle play");
    Media::State status = m_media->state();
    if (status == Media::PlayingState || status == Media::BufferingState)
    {
        m_media->pause();
    }
    else if(m_media->file().isNull())
    {
        loadTv(0, 0);
    }
    else
    {
        m_media->play();
    }
}

void PlayerWidget::toggleFullscreen()
{
    if (isFullScreen())    
        showPlayNormal();    
    else    
        showPlayFullscreen();    
}

void PlayerWidget::showPlayFullscreen()
{
    if (isFullScreen() == false)
    {
        parentWidget()->layout()->removeWidget(this);
        setWindowFlag(Qt::Window, true);
        showFullScreen();
        parentWidget()->hide();
    }
}

void PlayerWidget::showPlayNormal()
{
    if (isFullScreen())
    {
        showNormal();
        setWindowFlag(Qt::Window, false);
        parentWidget()->layout()->addWidget(this);
        parentWidget()->show();
        setFocus();
    }
}

bool PlayerWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_F || keyEvent->key() == Qt::Key_Enter)
            {
                toggleFullscreen();
            }
            else if (keyEvent->key() == Qt::Key_F11)
            {
                showPlayFullscreen();
            }
            else if (keyEvent->key() == Qt::Key_Escape)
            {
                showPlayNormal();
            }
            else if (keyEvent->key() == Qt::Key_Up)
            {
                if (m_media->state() == Media::PlayingState)
                {
                    int volume = m_media->volume() * 100;
                    if (volume < 100)
                    {
                        volume += 5;
                        if (volume > 100)
                            volume = 100;
                        m_media->setVolume(volume/100.0);
                    }
                    updateVolume(volume);
                }
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {

                if (m_media->state() == Media::PlayingState)
                {
                    int volume = m_media->volume() * 100;
                    if (volume > 0)
                    {
                        volume -= 5;
                        if (volume < 0)
                            volume = 0;
                        m_media->setVolume(volume/100.0);
                    }
                    updateVolume(volume);
                }
            }
            else if (keyEvent->key() == Qt::Key_M)
            {
                m_media->setVolumeMuted(!m_media->volumeMuted());
            }
            else if (keyEvent->key() == Qt::Key_P || keyEvent->key() == Qt::Key_Space)
            {
                toogle();
            }
            else if (keyEvent->key() == Qt::Key_T)
            {
                // toggleTopHint();
            }
            else
            {
                return false;
            }
            return true;
    }
    return QWidget::event(event);
}

bool PlayerWidget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_cover && event->type() == QEvent::MouseMove) {
        QMouseEvent *e = dynamic_cast<QMouseEvent*>(event);
        if (e->buttons().testFlag(Qt::LeftButton) == false) {
            showPlayButton();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PlayerWidget::printError(const QString &error)
{
    QString str;
    QDebug(&str) << "Player error: " << error;
    QDebug(&str) << "\r\n\tPlayer status: " << m_media->state();
//    QDebug(&str) << "\r\n\tPlayer MediaStatus: " << mediaPlayer->mediaStatus();
    QDebug(&str) << "\r\n\tSrc: " << m_media->file();
    Logger::instance().log(str.toStdString(), Logger::kLogLevelError);
//    mediaPlayer->play();
}


void PlayerWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (timerId > 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
        toggleFullscreen();
    }
}


#ifndef QT_NO_WHEELEVENT
void PlayerWidget::wheelEvent(QWheelEvent *event)
{
    if (m_media->state() == Media::PlayingState)
    {
        int volume = m_media->volume() * 100;
        if (event->angleDelta().y() > 0)
        {
            volume -= 5;
        }
        else
        {
            volume += 5;
        }
        if (volume < 0)
            volume = 0;
        if (volume > 100)
            volume = 100;
        m_media->setVolume(volume/100.0);
        updateVolume(volume);
    }
}
#endif

void PlayerWidget::enterEvent(QEvent *event)
{
    showPlayButton();
    QWidget::enterEvent(event);
}

void PlayerWidget::leaveEvent(QEvent *event)
{
    if (m_media->state() == Media::PlayingState)
        playButton->lower();
    QWidget::leaveEvent(event);
}

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (timerId == 0)
        {
            pressing = true;
            timerId = startTimer(700);
            current = QDateTime::currentMSecsSinceEpoch();
        }
        else
        {
            killTimer(timerId);
            timerId = 0;
        }
        mLastMousePosition = event->globalPos();
    }
}

void PlayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        if (timerId > 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
        parent->move(parent->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
//    else
//        showPlayButton();
}

void PlayerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    pressing = false;
}

void PlayerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void PlayerWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        QString url = event->mimeData()->text();
        m_media->play(url);
        Logger::instance().log("Switch to tv " + url.toStdString(), Logger::kLogLevelInfo);
    }
}

void PlayerWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PlayerWidget::resizeEvent(QResizeEvent *event) {
    volumeLabel->move((size().width() - volumeLabel->width()) / 2, (size().height() - volumeLabel->height()) / 2);
    playButton->move((size().width() - playButton->width()) / 2, (size().height() - playButton->height()) / 2);
    m_cover->setFixedSize(size());
    QWidget::resizeEvent(event);
}

void PlayerWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId && !pressing)
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        killTimer(timerId);
        timerId = 0;
        if (now - current < 800)
        {
            toogle();
        }
    }
}
