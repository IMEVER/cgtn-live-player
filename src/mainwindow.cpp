#include "mainwindow.h"
#include <QKeyEvent>
#include <QApplication>
#include <qapplication.h>
#include <singleapplication.h>
#include <QStackedLayout>
#include <QEvent>
#include <time.h>
#include <QStandardPaths>
#include <QFile>
#include "logger.h"
#include "conf.h"
#include <QClipboard>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    timerId = 0;
    volumeTimerId = 0;
    percent = 0;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    filterWidget = new QWidget;
    filterWidget->setContentsMargins(0, 0, 0, 0);
    filterWidget->setFixedWidth(160);
    QVBoxLayout *filterLayout = new QVBoxLayout;

    //Add filter search
    search = new QLineEdit();
    // search->clearFocus();
    // search->setFixedWidth(160);
    search->setPlaceholderText("请输入搜索关键字");
    search->installEventFilter(this);
    connect(search, &QLineEdit::textChanged, [=](const QString txt) {
        filterModel->setFilterFixedString(txt);
        treeMenu->expandAll();
    });

    filterLayout->addWidget(search);

    //Add tree list menu
    treeMenu = new QTreeView();
    treeMenu->setHeaderHidden(true);
    // treeMenu->setFixedWidth(160);
    treeMenu->installEventFilter(this);

    model = new QStandardItemModel();
    model->setColumnCount(1);
    QStandardItem *root = model->invisibleRootItem();

    int groupIndex = 0;
    foreach (const Group *group, *Conf::instance()->getJsonConf().groups)
    {
        int tvIndex = 0;
        QStandardItem *groupItem = new QStandardItem(group->name + "(" + QString::number(group->tvs->size()) + ")");
        foreach (const Item *item, *group->tvs)
        {
            QStandardItem *tvItem = new QStandardItem(item->title);
            QVector<int> indexes(2);
            indexes[0] = groupIndex;
            indexes[1] = tvIndex;
            tvItem->setData(QVariant::fromValue(indexes));
            groupItem->appendRow(tvItem);
            tvIndex++;
        }
        root->appendRow(groupItem);
        groupIndex++;
    }

    filterModel = new QSortFilterProxyModel;
    filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel->setFilterKeyColumn(0);
    filterModel->setRecursiveFilteringEnabled(true);
    filterModel->setSourceModel(model);

    treeMenu->setModel(filterModel);
    treeMenu->setEditTriggers(QTreeView::NoEditTriggers);
    treeMenu->setSelectionMode(QTreeView::SingleSelection);
    treeMenu->expandsOnDoubleClick();
    treeMenu->expand(filterModel->index(0, 0));
    treeMenu->setIndentation(6);
    connect(treeMenu, &QTreeView::doubleClicked, [=](const QModelIndex &index) {
        if (index.parent() != treeMenu->rootIndex())
        {
            // QVector<int> ss = (static_cast<QStandardItemModel *>(filterModel->sourceModel()))->itemFromIndex(index)->data().value<QVector<int>>();
            QVector<int> ss = (static_cast<QStandardItemModel *>(filterModel->sourceModel()))->itemFromIndex(filterModel->mapToSource(index))->data().value<QVector<int>>();
            loadTv(ss[0], ss[1]);
        }
    });

    filterLayout->addWidget(treeMenu);

    filterWidget->setLayout(filterLayout);

    layout->addWidget(filterWidget);

    QStackedLayout *mainLayout = new QStackedLayout();
    mainLayout->setMargin(0);
    mainLayout->setStackingMode(QStackedLayout::StackAll);

    layout->addLayout(mainLayout);

    volumeLabel = new QLabel(QString::number(50));
    volumeLabel->setFixedSize(QSize(140, 100));
    volumeLabel->setAlignment(Qt::AlignCenter);
    volumeLabel->setStyleSheet("border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0,1);");
    // volumeLabel->setWindowFlags(Qt::FramelessWindowHint);
    // volumeLabel->setAttribute(Qt::WA_TranslucentBackground);
    mainLayout->addWidget(volumeLabel);

    playButton = new QPushButton;
    playButton->setFixedSize(QSize(128, 128));
    playButton->setFocusPolicy(Qt::NoFocus);
    playButton->setStyleSheet("background-image:url(:/resource/loading.gif); border: 1px solid black; border-radius: 10px; font-size: 80px; color: white; background-color: rgba(0,0,0, 1)");
    mainLayout->addWidget(playButton);

    connect(playButton, &QPushButton::clicked, this, [=]() {
        switch (playStatus)
        {
        case playing:
        case loading:
            playStatus = stopped;
            mediaPlayer->pause();
            break;
        case stopped:
            playStatus = playing;
            mediaPlayer->play();
            break;
        }
        showPlayButton();
    });

    _videoWidget = new QVideoWidget;
    _videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    mainLayout->addWidget(_videoWidget);
    _videoWidget->setMouseTracking(true);
    _videoWidget->setFocusPolicy(Qt::StrongFocus);

    QWidget *centralWidget = new QWidget(this);
    //  centralWidget->setStyleSheet("background-color: grey");
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setMouseTracking(true);
    this->centralWidget()->setMouseTracking(true);

    mediaPlayer = new QMediaPlayer(nullptr, (QMediaPlayer::StreamPlayback));
    mediaPlayer->setPlaybackRate(1);
    mediaPlayer->setVideoOutput(_videoWidget);
    mediaPlayer->setVolume(50);

    ratio = static_cast<qreal>(404.7) / 720;

    connect(mediaPlayer, static_cast<void (QMediaPlayer::*)(const QMediaContent &)>(&QMediaPlayer::mediaChanged), [=](const QMediaContent &media) {
        Q_UNUSED(media);
        Logger::instance().log("Change to another media source");
        playStatus = loading;
        _videoWidget->setFocus();
        showPlayButton();        
    });
    connect(mediaPlayer, static_cast<void (QMediaObject::*)(const QString &, const QVariant &)>(&QMediaObject::metaDataChanged),
            [=](const QString &key, const QVariant &value) {
                if (key == "Resolution")
                {
                    QSize size = value.toSize();
                    //resize(size.width(), size.height());
                    Logger::instance().log("Video resolution by Width: " + std::to_string(size.width()) + ", Height: " + std::to_string(size.height()));
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
            showPlayButton();
            // playStatus = stopped;
        }
    });
    connect(mediaPlayer, static_cast<void (QMediaPlayer::*)(int percent)>(&QMediaPlayer::bufferStatusChanged), [=](int percent) {
        if (percent < 100)
        {
            Logger::instance().log("Loading " + std::to_string(percent) + "%");
            this->percent = percent;
            playStatus = loading;
            showPlayButton();
        }
        else
        {
            mediaPlayer->play();
            Logger::instance().log("Loading buffer 100%, now playing");
            playStatus = playing;
            playButton->hide();
        }
    });
    QObject::connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(printError(QMediaPlayer::Error)));

    setMinimumSize(QSize(720, 280));
    resize(880, 405);

    initContextMenu();
    setAcceptDrops(true);

    SingleApplication *app = static_cast<SingleApplication *>(QApplication::instance());
    Logger::instance().log("App name: " + app->applicationName().toStdString());
    QObject::connect(app, SIGNAL(instanceStarted()), this, SLOT(toFront()));

    volumeLabel->raise();
    playButton->raise();
    showPlayButton();
}

void MainWindow::toFront()
{
    Logger::instance().log("To Front called");
    raise();
    show();
    activateWindow();
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    Logger::instance().log("Right click trigger", Logger::kLogLevelInfo);

    QClipboard *clipboard = QApplication::clipboard();
    QString url = clipboard->text(QClipboard::Clipboard);
    bool hasUrl = !url.isEmpty() && !url.isNull() && url.startsWith("http");

    contextMenu->actions()[2]->setDisabled(!hasUrl);

    contextMenu->exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::initContextMenu()
{
    contextMenu = new QMenu(this);
    QMenu *shortMenu = contextMenu->addMenu("快捷键");
    shortMenu->addAction("Up | Down => 音量调节");
    shortMenu->addAction("F | DbClick => 全屏切换");
    shortMenu->addAction("F11 => 进入全屏");
    shortMenu->addAction("Esc => 退出全屏");
    shortMenu->addAction("P | Space | Click => 暂停播放");
    shortMenu->addAction("M => 静音");
    shortMenu->addAction("CTRL_S => 侧边栏");

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
        //            QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    });

    QAction *sideAction = new QAction("侧边栏", this);
    sideAction->setCheckable(true);
    sideAction->setChecked(true);
    // sideAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(sideAction, &QAction::trigger, [=]() {
        if (sideAction->isChecked())
        {
            filterWidget->show();
        }
        else
        {
            filterWidget->hide();
        }
    });
    contextMenu->addAction(sideAction);
    // contextMenu->addAction("侧边栏", this, [=](){
    // filterWidget->setHidden(!filterWidget->isHidden());
    // });

    // contextMenu->addAction("提示", this, [=](bool checked){
    //     toggleTopHint();
    // })->setCheckable(true);

    contextMenu->addAction("编辑播放列表", this, [=](){
        emit menuTrigger(2);
    });

    contextMenu->addAction("关于", this, [=]() {
        emit menuTrigger(1);
    });

    contextMenu->addAction("退出", qApp, SLOT(quit()));
    contextMenu->setStyleSheet("border-radius: 5px; background-color: grey");
}

void MainWindow::switchTv(QAction *action)
{
    QVector<int> ss = action->data().value<QVector<int>>();
    loadTv(ss[0], ss[1]);
}

void MainWindow::loadTv()
{
    loadTv(0, 0);
}

void MainWindow::loadTv(int groupIndex, int tvIndex)
{
    Item *url = Conf::instance()->getJsonConf().groups->at(groupIndex)->tvs->at(tvIndex);
    mediaPlayer->setMedia(QMediaContent(QUrl(url->url)));
    setWindowTitle("央视外语频道: " + url->title);
    Logger::instance().log("Switch to tv " + url->title.toStdString() + "\t" + url->url.toStdString(), Logger::kLogLevelInfo);
}

//TODO Not work
void MainWindow::toggleTopHint()
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags ^ Qt::WindowStaysOnTopHint ^ Qt::X11BypassWindowManagerHint);
    show();
}

void MainWindow::play(PlayStatus status)
{
    Logger::instance().log("toggle play");
    if (status == stopped)
    {
        if (playStatus == playing || playStatus == loading)
            mediaPlayer->pause();
    }
    else
    {
        if (playStatus == stopped)
            mediaPlayer->play();
    }

    playStatus = status;
}

MainWindow::~MainWindow()
{
    if (timerId > 0)
        killTimer(timerId);
}

void MainWindow::toogle()
{
    Logger::instance().log("toggle play");
    if (playStatus == playing || playStatus == loading)
    {
        mediaPlayer->pause();
        playStatus = stopped;
    }
    else
    {
        mediaPlayer->play();
        playStatus = playing;
    }

    emit toggleTrigger(playStatus);
}

void MainWindow::showVolumeLabel(int volume)
{
    if (!playButton->isHidden())
    {
        if (volumeTimerId > 0)
        {
            killTimer(volumeTimerId);
            volumeTimerId = 0;
        }
        // playButton->lower();
        playButton->hide();
    }

    volumeLabel->setText(QString::number(volume));
    // volumeLabel->raise();
    if(!volumeLabel->isHidden())
    {
        // return;
    }

    volumeLabel->show();
    volumeLabel->move((size().width() - volumeLabel->width()) / 2  + (filterWidget->isHidden() ? 0 : filterWidget->width() / 2), (size().height() - volumeLabel->height()) / 2);
    if (volumeTimerId > 0)
    {
        killTimer(volumeTimerId);
    }
    volumeTimerId = startTimer(1500);
}

void MainWindow::showPlayButton()
{
    if (!volumeLabel->isHidden())
    {
        if (volumeTimerId > 0)
        {
            killTimer(volumeTimerId);
            volumeTimerId = 0;
        }
        // volumeLabel->lower();
        volumeLabel->hide();
    }

    QString text;
    switch (playStatus)
    {
        case playing:
            playButton->setStyleSheet("background-image:url(:/resource/pause.png);");
            break;
        case loading:
            playButton->setStyleSheet("background-image:url(:/resource/loading.gif);");
            text = QString::number(percent) + "%";
            break;
        case stopped:
            playButton->setStyleSheet("background-image:url(:/resource/play.png);");
            break;
        default:
            break;
    }

    if (!playButton->isHidden())
    {
        // return;
    }
    
    playButton->move((size().width() - playButton->width()) / 2 + (filterWidget->isHidden() ? 0 : filterWidget->width() / 2), (size().height() - playButton->height()) / 2);
    playButton->setText(text);
    // playButton->raise();
    playButton->show();

    if (playStatus == playing && volumeTimerId == 0)
    {
        volumeTimerId = startTimer(5000);
    }
    else if (playStatus != playing && volumeTimerId > 0)
    {
        killTimer(volumeTimerId);
        volumeTimerId = 0;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == filterWidget || watched == search)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->modifiers() == Qt::CTRL)
            {
                event->ignore();
                return true;
            }
        }
    }
    // return false;
    return QMainWindow::eventFilter(watched, event);
}

bool MainWindow::event(QEvent *event)
{
    Q_UNUSED(event);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_F || keyEvent->key() == Qt::Key_Enter)
            {
                if (isFullScreen())
                {
                    filterWidget->show();
                    showNormal();
                }
                else
                {
                    filterWidget->hide();
                    showFullScreen();
                }
            }
            else if (keyEvent->key() == Qt::Key_F11)
            {
                if (isFullScreen() == false)
                {
                    filterWidget->hide();
                    showFullScreen();
                }
            }
            else if (keyEvent->key() == Qt::Key_Escape)
            {
                if (isFullScreen())
                {
                    filterWidget->show();
                    showNormal();
                }
            }
            else if (keyEvent->key() == Qt::Key_Up)
            {
                if (playStatus == playing)
                {
                    int volume = mediaPlayer->volume();
                    if (volume < 100)
                    {
                        volume += 5;
                        if (volume > 100)
                            volume = 100;
                        mediaPlayer->setVolume(volume);
                    }
                    playButton->hide();
                    showVolumeLabel(volume);
                }
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {

                if (playStatus == playing)
                {
                    int volume = mediaPlayer->volume();
                    if (volume > 0)
                    {
                        volume -= 5;
                        if (volume < 0)
                            volume = 0;
                        mediaPlayer->setVolume(volume);
                    }
                    playButton->hide();
                    showVolumeLabel(volume);
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
            else if (keyEvent->key() == Qt::Key_S)
            {
                contextMenu->actions().at(4)->setChecked(!contextMenu->actions().at(4)->isChecked());
                filterWidget->setHidden(!filterWidget->isHidden());
            }
            return true;
    }
    return QMainWindow::event(event);
}

#ifndef QT_NO_WHEELEVENT
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (playStatus == playing)
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
        playButton->hide();
        emit volumeChanged(volume);
    }
}
#endif

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QSize windowSize = size();
    //    QSize oldSize = event->oldSize();
    //    QSize newSize = event->size();
    QSize fixedSize(windowSize.width(), windowSize.height());

    //    if(newSize.width() != oldSize.width())
    //    {
    //        fixedSize.setWidth(newSize.width());
    //        fixedSize.setHeight((int)(newSize.width() * ratio));
    //        qDebug()<<"Resize to width: "<<fixedSize.width()<<", height: "<<fixedSize.height()<<", by ratio: "<<ratio<<", "<<(int)(newSize.width() * ratio);
    //        resize(fixedSize);
    //    }
    // volumeLabel->move(size().width() / 2 - 70 + 80, size().height() / 2 - 60);
    // volumeLabel->setGeometry(size().width() / 2 - 70 + 80, size().height() / 2 - 60, volumeLabel->x(), volumeLabel->y());
    emit windowResize(fixedSize);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
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

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        if (timerId > 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
        this->move(this->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
    else
    {
        showPlayButton();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    pressing = false;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == volumeTimerId)
    {
        killTimer(volumeTimerId);
        volumeTimerId = 0;
        if (!volumeLabel->isHidden())
        {
            // volumeLabel->lower();
            volumeLabel->hide();
        }
        if (!playButton->isHidden())
        {
            // playButton->lower();
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

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (timerId > 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
        if (isFullScreen())
        {
            filterWidget->show();
            showNormal();
        }
        else
        {
            filterWidget->hide();
            showFullScreen();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QClipboard *clipboard = QApplication::clipboard();
    QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        QUrl url = QUrl(event->mimeData()->text());
        mediaPlayer->setMedia(url);
        Logger::instance().log("Switch to tv " + url.toString().toStdString(), Logger::kLogLevelInfo);
    }
}

void MainWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    playButton->show();
}

void MainWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (playStatus == playing)
        playButton->hide();
}

void MainWindow::printError(QMediaPlayer::Error error)
{
    QString str;
    QDebug(&str) << "Player error: " << error;
    QDebug(&str) << "\r\n\tPlayer status: " << mediaPlayer->state();
    QDebug(&str) << "\r\n\tPlayer MediaStatus: " << mediaPlayer->mediaStatus();
    QDebug(&str) << "\r\n\tSrc: " << mediaPlayer->media().canonicalUrl().url();
    Logger::instance().log(str.toStdString(), Logger::kLogLevelError);
    mediaPlayer->play();
}

void MainWindow::updateGroupName(int index)
{
    model->item(index, 0)->setText(Conf::instance()->getGroupList()->at(index));
}

void MainWindow::updateTvTitle(int groupIndex, int tvIndex)
{
    model->item(groupIndex, 0)->child(tvIndex, 0)->setText(Conf::instance()->getJsonConf().groups->at(groupIndex)->tvs->at(tvIndex)->title);
}