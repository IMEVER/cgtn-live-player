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
// #include <QGraphicsBlurEffect>

#include "channelconf.h"
#include "conf.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // QGraphicsBlurEffect *effect = new QGraphicsBlurEffect(this);

    // setGraphicsEffect(effect);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // setAttribute(Qt::WA_TranslucentBackground, true);
    // setAutoFillBackground(true);

    filterWidget = new FilterWidget(this);
    filterWidget->setContentsMargins(0, 0, 0, 0);
    filterWidget->setFixedWidth(160);

    filterWidget->setVisible(Conf::instance()->isSidebarShow());

    connect(filterWidget, &FilterWidget::tvItemDoubleClicked, [=](int groupIndex, int tvIndex){
        _playerWidget->loadTv(groupIndex, tvIndex);
    });
    layout->addWidget(filterWidget);

    _playerWidget = new PlayerWidget(Conf::instance()->isSidebarShow(), this);
    connect(_playerWidget, &PlayerWidget::menuTrigger, [=](int id){
        emit openWindowTrigger(id);
    });
    connect(_playerWidget, &PlayerWidget::toggleFilterWidget, [=](){
        bool isShow = !filterWidget->isHidden();
        filterWidget->setHidden(isShow);
        (menuBar()->actions().at(2)->menu())->actions()[0]->setChecked(!isShow);
        Conf::instance()->setSidebarShow(!isShow);
    });
    layout->addWidget(_playerWidget);

    setWindowIcon(QIcon(":/resource/cgtn-live-player.svg"));

    setMinimumSize(QSize(720, 280));
    resize(880, 405);

    SingleApplication *app = static_cast<SingleApplication *>(QApplication::instance());
    Logger::instance().log("App name: " + app->applicationName().toStdString());
    QObject::connect(app, SIGNAL(instanceStarted()), this, SLOT(toFront()));

    initMenubar();
}

void MainWindow::initMenubar()
{
    QMenu *file = menuBar()->addMenu("文件(&F)");

    file->addAction("打开(F)", this, [this](){
        QFileDialog dialog(this);
        dialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first());
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter("Video (*.mp4 *.mkv)");
        if (dialog.exec())
        {
            QUrl url = QUrl::fromLocalFile(dialog.selectedFiles().first());
            _playerWidget->setMediaUrl(url);
            Logger::instance().log("Switch to local video: " + url.toString().toStdString());
        }
    });

    file->addAction("粘贴在线播放地址", this, [=]() {
        QClipboard *clipboard = QApplication::clipboard();
        QString url = clipboard->text(QClipboard::Clipboard);
        if (!url.isEmpty() && !url.isNull())
        {
            _playerWidget->setMediaUrl(QUrl(url));
            Logger::instance().log("Switch to tv " + url.toStdString(), Logger::kLogLevelInfo);
        }
    });

    file->addAction("拷贝当前播放地址", this, [=]() {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(_playerWidget->getMediaUrl().url(), QClipboard::Clipboard);
        //QApplication::instance()->sendEvent(clipboard, new QEvent(QEvent::Clipboard));
    });


    file->addAction("退出(q)", qApp, SLOT(quit()));

    QMenu *tvMenu = menuBar()->addMenu("电台(&T)");

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
    connect(tvMenu, &QMenu::triggered, this, [this](QAction *action){
        _playerWidget->switchTv(action);
    });

    QMenu *option = menuBar()->addMenu("选项(&O)");

    QAction *sideBarAction = new QAction("侧边栏(S)");
    sideBarAction->setCheckable(true);
    sideBarAction->setChecked(Conf::instance()->isSidebarShow());
    connect(sideBarAction, &QAction::triggered, this, [this](bool checked){
        filterWidget->setHidden(!checked);
        _playerWidget->setSideActionMenuChecked(checked);
        Conf::instance()->setSidebarShow(checked);
    });
    option->addAction(sideBarAction);

    option->addAction("编辑播放列表(E)", this, [this](){
        emit openWindowTrigger(2);
    });

    QMenu *help = menuBar()->addMenu("帮助(&H)");

    QMenu *key = help->addMenu("快捷键(K)");
    key->addAction("Up | Down => 音量调节");
    key->addAction("F | DbClick => 全屏切换");
    key->addAction("F11 => 进入全屏");
    key->addAction("Esc => 退出全屏");
    key->addAction("P | Space | Click => 暂停播放");
    key->addAction("M => 静音");
    key->addAction("CTRL_S => 侧边栏");

    help->addAction("关于(A)", this, [this](){
        emit openWindowTrigger(1);
    });
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

MainWindow::~MainWindow()
{

}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_S)
        {
            (menuBar()->actions().at(2)->menu())->actions()[0]->trigger();
            return true;
        }
    }

    return QMainWindow::event(event);
}

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