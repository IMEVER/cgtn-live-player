#include "playerWidget.h"
#include <QStackedLayout>

#include "logger.h"
#include "conf.h"

#include <QVideoWidget>
#include <QTimerEvent>
#include <QDateTime>
#include <QApplication>
#include <QContextMenuEvent>
#include <QClipboard>
#include <QMimeData>
#include <QVariant>

PlayerWidget::PlayerWidget(QWidget *parent): QWidget(parent)
{
    this->parent = parent;
    QStackedLayout *layout = new QStackedLayout;
    layout->setStackingMode(QStackedLayout::StackAll);

    setLayout(layout);

    setStyleSheet("{background-color: rgba(0,0,0,255);}");

    volumeLabel = new QLabel(QString::number(50), this);
    volumeLabel->setFixedSize(QSize(140, 100));
    volumeLabel->setAlignment(Qt::AlignCenter);
    volumeLabel->setAutoFillBackground(true);
    volumeLabel->setStyleSheet("QLabel {border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0, 0, 0, 255) }");
    // QPalette palette;
    // palette.setColor(QPalette::Background, QColor::fromRgba64(0, 0, 0, 100));
    // volumeLabel->setPalette(palette);
    layout->addWidget(volumeLabel);

    playButton = new QPushButton(this);
    playButton->setFixedSize(QSize(128, 128));
    playButton->setFocusPolicy(Qt::NoFocus);
    playButton->setStyleSheet("border-image:url(:/resource/loading.gif); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 1)");
    layout->addWidget(playButton);

    connect(playButton, &QPushButton::clicked, this, &PlayerWidget::toogle);

    QVideoWidget *_videoWidget = new QVideoWidget;
    _videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    layout->addWidget(_videoWidget);
    _videoWidget->setMouseTracking(true);
    _videoWidget->setFocusPolicy(Qt::StrongFocus);

    mediaPlayer = new QMediaPlayer(nullptr, (QMediaPlayer::StreamPlayback));
    mediaPlayer->setPlaybackRate(1);
    mediaPlayer->setVideoOutput(_videoWidget);
    mediaPlayer->setVolume(50);

    ratio = static_cast<qreal>(404.7) / 720;

    connect(mediaPlayer, static_cast<void (QMediaPlayer::*)(const QMediaContent &)>(&QMediaPlayer::mediaChanged), [=](const QMediaContent &media) {
        // media.isNull()
        Logger::instance().log("Change to another media source");
        setFocus();
        if(!media.canonicalUrl().isLocalFile())
        {
            updatePlay(loading);
        }
    });
    connect(mediaPlayer, static_cast<void (QMediaObject::*)(const QString &, const QVariant &)>(&QMediaObject::metaDataChanged),
            [=](const QString &key, const QVariant &value) {
                if (key == "Resolution")
                {
                    QSize size = value.toSize();
                    Logger::instance().log("Video resolution by Width: " + std::to_string(size.width()) + ", Height: " + std::to_string(size.height()));
                    updatePlay(playing);
                    mediaPlayer->play();
                }
            });
    connect(mediaPlayer, static_cast<void (QMediaPlayer::*)(bool bavial)>(&QMediaPlayer::videoAvailableChanged), [=](bool bavial) {
        if (bavial)
        {
            Logger::instance().log("Start loading video");
        }
        else
        {
            Logger::instance().log("Video not available");
            // showPlayButton();
        }
    });
    connect(mediaPlayer, static_cast<void (QMediaPlayer::*)(int percent)>(&QMediaPlayer::bufferStatusChanged), [=](int percent) {
        if (percent < 100)
        {
            Logger::instance().log("Loading " + std::to_string(percent) + "%");
            updatePlay(loading, percent);
        }
        else
        {
            mediaPlayer->play();
            Logger::instance().log("Loading buffer 100%, now playing");
            updatePlay(playing);
        }
    });
    QObject::connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(printError(QMediaPlayer::Error)));


    initContextMenu();


    setAcceptDrops(true);

    volumeLabel->hide();
    playButton->hide();
    setMouseTracking(true);
}

PlayerWidget::~PlayerWidget()
{
    if (timerId > 0)
        killTimer(timerId);

    if (volumeTimerId > 0)
    {
        killTimer(volumeTimerId);
    }
}

#ifndef QT_NO_CONTEXTMENU
void PlayerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Logger::instance().log("Right click trigger", Logger::kLogLevelInfo);

    QClipboard *clipboard = QApplication::clipboard();
    QString url = clipboard->text(QClipboard::Clipboard);
    bool hasUrl = !url.isEmpty() && !url.isNull() && url.startsWith("http");

    contextMenu->actions()[2]->setDisabled(!hasUrl);

    contextMenu->exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void PlayerWidget::initContextMenu()
{
    contextMenu = new QMenu();
    QMenu *shortMenu = contextMenu->addMenu("快捷键");
    shortMenu->addAction("Up | Down => 音量调节");
    shortMenu->addAction("F | DbClick => 全屏切换");
    shortMenu->addAction("F11 => 进入全屏");
    shortMenu->addAction("Esc => 退出全屏");
    shortMenu->addAction("P | Space | Click => 暂停播放");
    shortMenu->addAction("M => 静音");
    shortMenu->addAction("S => 侧边栏");

    QMenu *tvMenu = contextMenu->addMenu("电台");
    QActionGroup *groups = new QActionGroup(this);

    bool isFirst = true;
    int groupIndex = 0;
    foreach (Group *group, *Conf::instance()->getJsonConf().groups)
    {
        for (int tvIndex = 0, len = group->tvs->size(); tvIndex < len; tvIndex++)
        {
            Item *tv = group->tvs->at(tvIndex);
            QAction *action = new QAction(tv->title, this);
            QVector<int> indexes(2);
            indexes[0] = groupIndex;
            indexes[1] = tvIndex;
            action->setData(QVariant::fromValue(indexes));
            action->setCheckable(true);
            if (isFirst)
            {
                action->setChecked(true);
                isFirst = false;
            }
            groups->addAction(action);
            tvMenu->addAction(action);
        }
        // tvMenu->addSeparator();
        tvMenu->addSection(group->name);
        groupIndex++;
    }
    connect(tvMenu, SIGNAL(triggered(QAction *)), this, SLOT(switchTv(QAction *)));

    contextMenu->addAction("粘贴在线播放地址", this, [=]() {
        QClipboard *clipboard = QApplication::clipboard();
        QString url = clipboard->text(QClipboard::Clipboard);
        if (!url.isEmpty() && !url.isNull())
        {
            mediaPlayer->setMedia(QUrl(url));
            Logger::instance().log("Switch to tv " + url.toStdString(), Logger::kLogLevelInfo);
        }
    });

    contextMenu->addAction("拷贝当前播放地址", this, [=]() {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(mediaPlayer->media().canonicalUrl().url());
        //QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    });

    QAction *sideAction = new QAction("侧边栏", this);
    sideAction->setCheckable(true);
    sideAction->setChecked(true);
    // sideAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(sideAction, &QAction::changed, this, [=]() {
        emit toggleFilterWidget();
    });
    contextMenu->addAction(sideAction);

    // contextMenu->addAction("提示", this, [=](bool checked){
    //     toggleTopHint();
    // })->setCheckable(true);

    contextMenu->addAction("编辑播放列表", this, [=](){
        emit menuTrigger(2);
    });

    contextMenu->addAction("关于", this, [=]() {
        emit menuTrigger(1);
    });

    contextMenu->setStyleSheet("border-radius: 5px; background-color: grey");
}

void PlayerWidget::setSideActionMenuChecked(bool checked)
{
    contextMenu->actions().at(4)->setChecked(checked);
}

void PlayerWidget::updateVolume(int volume)
{
    QVariant v = volumeLabel->property("volume");
    int old = v.isValid() ? v.toInt() : 0;
    if(old != volume)
    {
        volumeLabel->setProperty("volume", volume);
        volumeLabel->setText(QString::number(volume));
    }
    showVolumeLabel();
}

void PlayerWidget::showVolumeLabel()
{
    if (!playButton->isHidden())
    {
        if (volumeTimerId > 0)
        {
            killTimer(volumeTimerId);
            volumeTimerId = 0;
        }
        playButton->hide();
    }

    if(volumeLabel->isHidden())
    {
        volumeLabel->show();
        Logger::instance().log("x: " + std::to_string(volumeLabel->x()) + " y: " + std::to_string(volumeLabel->y()));
        if (volumeTimerId > 0)
        {
            killTimer(volumeTimerId);
        }
        volumeTimerId = startTimer(1500);
    }
    volumeLabel->move((size().width() - volumeLabel->width()) / 2, (size().height() - volumeLabel->height()) / 2);
}

PlayerWidget::PlayStatus PlayerWidget::getPlayStatus()
{
    QVariant v = playButton->property("status");
    return v.isValid() ? (PlayStatus)v.toInt() : stopped;
}

void PlayerWidget::updatePlay(PlayStatus status, int percent)
{
    QVariant v = playButton->property("status");
    PlayStatus oldStatus = v.isValid() ? (PlayStatus)v.toInt() : stopped;
    if (oldStatus != status)
    {
        QString img;
        playButton->setProperty("status", status);
        switch (status)
        {
            case playing:
                img = "pause.png";
                break;
            case loading:
                img = "loading.gif";                
                break;
            case stopped:
                img = "play.png";
                break;
            default:
                break;
        }
        playButton->setStyleSheet(QString("background-image:url(:/resource/%1);").arg(img));
    }

    if (status == loading)
    {
        v = playButton->property("percent");
        int oldPercent = v.isValid() ? v.toInt() : -1;
        if (oldPercent != percent)
        {
            playButton->setProperty("percent", percent);
            playButton->setText(QString::number(percent) + "%");
        }
        showPlayButton();
    }
    else if(oldStatus != status)
    {
        playButton->setText("");
        if (status == playing)
        {
            playButton->hide();

            playButton->hide();

            if (volumeTimerId > 0)
            {
                killTimer(volumeTimerId);
                volumeTimerId = 0;
            }            
        }
        else
        {
            showPlayButton();
        }        
    }
}

void PlayerWidget::showPlayButton()
{
    if (!volumeLabel->isHidden())
    {
        if (volumeTimerId > 0)
        {
            killTimer(volumeTimerId);
            volumeTimerId = 0;
        }
        volumeLabel->hide();
    }

    PlayStatus status = getPlayStatus();
    if(playButton->isHidden())
    {
        playButton->show();

        if (status == playing && volumeTimerId == 0)
        {
            volumeTimerId = startTimer(5000);
        }
    }

    playButton->move((size().width() - playButton->width()) / 2, (size().height() - playButton->height()) / 2);        
    
    if ((status == loading || status == stopped) && volumeTimerId > 0)
    {
        killTimer(volumeTimerId);
        volumeTimerId = 0;
    }
}

void PlayerWidget::loadTv(int groupIndex, int tvIndex)
{
    Item *url = Conf::instance()->getJsonConf().groups->at(groupIndex)->tvs->at(tvIndex);
    mediaPlayer->setMedia(QMediaContent(QUrl(url->url)));
    parent->setWindowTitle("央视外语频道: " + url->title);
    Logger::instance().log("Switch to tv " + url->title.toStdString() + "\t" + url->url.toStdString(), Logger::kLogLevelInfo);
}

void PlayerWidget::switchTv(QAction *action)
{
    QVector<int> ss = action->data().value<QVector<int>>();
    loadTv(ss[0], ss[1]);
}

void PlayerWidget::setMediaUrl(QUrl url)
{
    mediaPlayer->setMedia(url);
}

QUrl PlayerWidget::getMediaUrl()
{
    return mediaPlayer->media().canonicalUrl();
}

void PlayerWidget::toogle()
{
    Logger::instance().log("toggle play");
    PlayStatus status = getPlayStatus();
    if (status == playing || status == loading)
    {
        mediaPlayer->pause();
        updatePlay(stopped);
    }
    else
    {
        mediaPlayer->play();
        updatePlay(playing);
    }
}

void PlayerWidget::toggleFullscreen()
{
    if (isFullScreen())
    {
        showPlayNormal();
    }
    else
    {
        showPlayFullscreen();
    }
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
                if (getPlayStatus() == playing)
                {
                    int volume = mediaPlayer->volume();
                    if (volume < 100)
                    {
                        volume += 5;
                        if (volume > 100)
                            volume = 100;
                        mediaPlayer->setVolume(volume);
                    }
                    updateVolume(volume);
                }
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {

                if (getPlayStatus() == playing)
                {
                    int volume = mediaPlayer->volume();
                    if (volume > 0)
                    {
                        volume -= 5;
                        if (volume < 0)
                            volume = 0;
                        mediaPlayer->setVolume(volume);
                    }
                    updateVolume(volume);
                }
            }
            else if (keyEvent->key() == Qt::Key_M)
            {
                mediaPlayer->setMuted(!mediaPlayer->isMuted());
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

void PlayerWidget::printError(QMediaPlayer::Error error)
{
    QString str;
    QDebug(&str) << "Player error: " << error;
    QDebug(&str) << "\r\n\tPlayer status: " << mediaPlayer->state();
    QDebug(&str) << "\r\n\tPlayer MediaStatus: " << mediaPlayer->mediaStatus();
    QDebug(&str) << "\r\n\tSrc: " << mediaPlayer->media().canonicalUrl().url();
    Logger::instance().log(str.toStdString(), Logger::kLogLevelError);
    mediaPlayer->play();
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
    if (getPlayStatus() == playing)
    {
        int volume = mediaPlayer->volume();
        if (event->delta() < 0)
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
        mediaPlayer->setVolume(volume);
        updateVolume(volume);
    }
}
#endif

void PlayerWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (getPlayStatus() == playing)
        playButton->hide();
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
    else
    {
        showPlayButton();
    }
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
        QUrl url = QUrl(event->mimeData()->text());
        mediaPlayer->setMedia(url);
        Logger::instance().log("Switch to tv " + url.toString().toStdString(), Logger::kLogLevelInfo);
    }
}

void PlayerWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (playButton->isVisible())
    {
        playButton->hide();
        playButton->hide();
        showPlayButton();
    }
}

void PlayerWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == volumeTimerId)
    {
        killTimer(volumeTimerId);
        volumeTimerId = 0;
        if (!volumeLabel->isHidden())
        {
            volumeLabel->hide();
        }
        if (!playButton->isHidden())
        {
            playButton->hide();
            playButton->hide();
        }
    }
    else if (event->timerId() == timerId && !pressing)
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
