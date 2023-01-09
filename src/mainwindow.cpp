#include "mainwindow.h"

#include <QKeyEvent>
#include <QApplication>
#include <QIcon>
#include <qapplication.h>
#include <singleapplication.h>
#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QMenuBar>
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>

#include "channelconf.h"
#include "conf.h"
#include "logger.h"
#include "mpris.h"
#include "mediampv.h"

QMap<QString, QAction*> actionMap;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(300);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::resizeVideo);
    keepWidth = true;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    initMenubar();

    filterWidget = new FilterWidget(this);
    filterWidget->setContentsMargins(0, 0, 0, 0);
    filterWidget->setFixedWidth(160);

    filterWidget->setVisible(Conf::instance()->isSidebarShow());

    connect(filterWidget, &FilterWidget::tvItemDoubleClicked, [this](int groupIndex, int tvIndex){
        _playerWidget->loadTv(groupIndex, tvIndex);
    });
    layout->addWidget(filterWidget);

    media = new MediaMPV();

    _playerWidget = new PlayerWidget(this);
    _playerWidget->setMedia(media);
    connect(media, &Media::stateChanged, [this](Media::State state){
        QVariantMap map;
        switch (state) {
        case Media::LoadingState:
            map.insert("Metadata", true);
            break;
        case Media::PlayingState:
            map.insert("PlaybackStatus", "Playing");
            break;
        case Media::PausedState:
            map.insert("PlaybackStatus", "Paused");
            break;
        case Media::StoppedState:
        default:
            map.insert("PlaybackStatus", "Stopped");
            break;
        }
        m_playerAdaptor->notifyPropertyChanged(map, {});
    });

    layout->addWidget(_playerWidget);

    setWindowIcon(QIcon(":/resource/cgtn-live-player.svg"));

    setMinimumSize(QSize(720, 280));
    resize(880, 405);
    resizeVideo();
    SingleApplication *app = static_cast<SingleApplication *>(QApplication::instance());
    Logger::instance().log("App name: " + app->applicationName().toStdString());
    connect(app, SIGNAL(instanceStarted()), this, SLOT(toFront()));

    QTimer::singleShot(1000, [this]{
        new MediaPlayer2Adaptor(this);
        QDBusConnection::sessionBus().registerService("org.mpris.MediaPlayer2.cgtn");
        m_playerAdaptor = new PlayerAdaptor(this);
        QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", this);
    });
}

void MainWindow::initMenubar()
{
    QMenu *file = menuBar()->addMenu("文件(&F)");

    file->addAction("打开(F)", this, [this]{
        QFileDialog dialog(this);
        dialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first());
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter("Video (*.mp4 *.mkv)");
        if (dialog.exec())
        {
            QString url = dialog.selectedFiles().first();
            media->play(url);
            Logger::instance().log("Switch to local video: " + url.toStdString());
        }
    });

    QAction *action = file->addAction("粘贴在线播放地址", this, [this] {
        QClipboard *clipboard = QApplication::clipboard();
        QString url = clipboard->text(QClipboard::Clipboard);
        if (!url.isEmpty() && !url.isNull())
        {
            media->play(url);
            Logger::instance().log("Switch to tv " + url.toStdString(), Logger::kLogLevelInfo);
        }
    });

    actionMap.insert("pasteUrl", action);

    action = file->addAction("拷贝当前播放地址", this, [this] {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(media->file(), QClipboard::Clipboard);
        //QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    });
    actionMap.insert("copyUrl", action);

    QAction *quitAction = file->addAction("退出(q)", qApp, SLOT(quit()));
    quitAction->setMenuRole(QAction::QuitRole);

    connect(file, &QMenu::aboutToShow, [file, this]{
        QClipboard *clipboard = QApplication::clipboard();
        QString url = clipboard->text(QClipboard::Clipboard);
        bool hasUrl = !url.isEmpty() && !url.isNull() && url.startsWith("http");

        file->actions()[1]->setDisabled(!hasUrl);
        file->actions().at(2)->setDisabled(media->file().isEmpty());
    });


    QMenu *tvMenu = menuBar()->addMenu("电台(&T)");
    actionMap.insert("tv", tvMenu->menuAction());
    QActionGroup *groups = new QActionGroup(this);

    bool isFirst = true;
    int groupIndex = 0;
    foreach (Group *group, *ChannelConf::instance()->getJsonConf().groups)
    {
        // tvMenu->addSection(group->name)->setText(QString("----?----").arg(group->name));
        tvMenu->addAction(group->name)->setEnabled(false);
        for (int tvIndex = 0, len = group->tvs->size(); tvIndex < len; tvIndex++)
        {
            Channel *tv = group->tvs->at(tvIndex);
            QAction *action = new QAction("    " + tv->title, this);
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
        groupIndex++;
    }
    connect(tvMenu, &QMenu::triggered, [this](QAction *action){_playerWidget->switchTv(action);});

    QMenu *option = menuBar()->addMenu("选项(&O)");

    QAction *audioOnly = option->addAction("仅声音");
    audioOnly->setCheckable(true);
    connect(audioOnly, &QAction::triggered, [this](bool checked){_playerWidget->setAudioOnly(checked);});

    QAction *sideBarAction = new QAction("侧边栏(S)");
    sideBarAction->setCheckable(true);
    sideBarAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    sideBarAction->setChecked(Conf::instance()->isSidebarShow());
    connect(sideBarAction, &QAction::triggered, this, [this](bool checked){
        filterWidget->setHidden(!checked);
        keepWidth = true;
        resizeVideo();
        Conf::instance()->setSidebarShow(checked);
    });
    option->addAction(sideBarAction);
    actionMap.insert("sidebar", sideBarAction);

    option->addSeparator();

    action = option->addAction("编辑播放列表(E)", this, [this]{
        emit openWindowTrigger(2);
    });
    actionMap.insert("edit", action);

    QMenu *help = menuBar()->addMenu("帮助(&H)");

    QMenu *key = help->addMenu("快捷键(K)");
    key->addAction("Up | Down => 音量调节");
    key->addAction("F | DbClick => 全屏切换");
    key->addAction("F11 => 进入全屏");
    key->addAction("Esc => 退出全屏");
    key->addAction("P | Space | Click => 暂停播放");
    key->addAction("M => 静音");
    key->addAction("CTRL_S => 侧边栏");
    actionMap.insert("shortcut", key->menuAction());

    QAction *aboutAction = help->addAction("关于(A)", [this]{
        emit openWindowTrigger(1);
    });
    aboutAction->setMenuRole(QAction::AboutRole);
    actionMap.insert("about", aboutAction);
}

void MainWindow::raiseApp()
{
    activateWindow();
    raise();
}

void MainWindow::pause()
{
   _playerWidget->toogle();
}

QVariantMap MainWindow::metaData()
{
    QVariantMap map;
        map.insert("mpris:length", 0);
        map.insert("xesam:album", "央视外语频道");
        map.insert("mpris:artUrl", "/opt/apps/me.imever.cgtn-live-player/entries/icons/hicolor/scalable/apps/cgtn-live-player.svg");
        map.insert("xesam:title", windowTitle());
    return map;
}

QString MainWindow::playbackStatus()
{
   Media::State status = media->state();
   switch (status) {
   case Media::PlayingState:
       return "Playing";
       break;
   case Media::StoppedState:
       return "Stopped";
       break;
   case Media::BufferingState:
   case Media::LoadingState:
       return "Loading";
   default:
       return "Stopped";
       break;
   }
}

void MainWindow::toFront()
{
    Logger::instance().log("To Front called");
    raise();
    show();
    activateWindow();
}

//TODO Not work
void MainWindow::toggleTopHint()
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags ^ Qt::WindowStaysOnTopHint ^ Qt::X11BypassWindowManagerHint);
    show();
}

void MainWindow::play()
{
    _playerWidget->loadTv(0, 0);
}

QAction *MainWindow::getAction(QString title)
{
    return actionMap.value(title);
}

MainWindow::~MainWindow()
{

}

void MainWindow::resizeVideo()
{
    if(!this->isMaximized() && !this->isFullScreen())
    {
        const double ratio = 16. / 9.;
        const double w = (double)(filterWidget->isVisible() ? width() - filterWidget->width() : width());
//        _playerWidget->setFixedWidth(w);
        const double h = (double)height(); //_playerWidget->height();
        const double currentVideoRatio = w / h;

        if(ratio != currentVideoRatio)
        {
            if(keepWidth)
                resize(width(), qRound(w/ratio));
            else
                resize(qRound(h * ratio) + (filterWidget->isVisible() ? filterWidget->width() : 0), height());
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
//    QMainWindow::resizeEvent(event);
    keepWidth = qAbs(event->size().width() - event->oldSize().width()) > qAbs(event->size().height() - event->oldSize().height());
    m_timer->start();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 200));  //QColor最后一个参数80代表背景的透明度
}

void MainWindow::updateGroupName(int index)
{
    filterWidget->updateGroupName(index);
}

void MainWindow::updateTvTitle(int groupIndex, int tvIndex)
{
    filterWidget->updateTvTitle(groupIndex, tvIndex);
}
