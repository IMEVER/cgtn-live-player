#include "listGroup.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>
#include "channelconf.h"
#include "logger.h"

ListGroup::ListGroup(QWidget *parent) : QMainWindow(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *toolBar = new QHBoxLayout(this);
    toolBar->setAlignment(Qt::AlignLeft);
    QPushButton *addButton = new QPushButton(this);
    addButton->setText("添加分组");
    addButton->setFixedWidth(100);
    connect(addButton, &QPushButton::clicked, [=](){
        listWidget->addItem("新分组名称");
        int count = listWidget->count();
        QListWidgetItem *item = listWidget->item(count -1);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        listWidget->editItem(item);
    });
    toolBar->addWidget(addButton);
    // layout->addLayout(toolBar);

    listWidget = new QListWidget(this);
    listWidget->setSpacing(10);
    listWidget->setAlternatingRowColors(true);
    listWidget->setStyleSheet("QListWidget::item { border-bottom: 1px solid black; }");
    listWidget->setSelectionBehavior(QTableWidget::SelectRows);
    listWidget->setSelectionMode(QTableWidget::SingleSelection);
    listWidget->setEditTriggers(QListWidget::DoubleClicked);

    foreach(QString group, *ChannelConf::instance()->getGroupList())
    {
        listWidget->addItem(group);
    }

    for(int i=0, len=listWidget->count(); i<len; i++)
    {
        listWidget->item(i)->setFlags(listWidget->item(i)->flags() | Qt::ItemIsEditable);
    }

    connect(listWidget->itemDelegate(), &QAbstractItemDelegate::closeEditor, [=](QWidget *editor, QAbstractItemDelegate::EndEditHint hint){
        Q_UNUSED(hint);
        QString groupName = reinterpret_cast<QLineEdit *>(editor)->text();
        int row = listWidget->currentRow();
        ChannelConf *conf = ChannelConf::instance();

        if (row <= conf->getGroupList()->size() -1 && groupName.compare(conf->getGroupList()->at(row)) == 0)
        {
            return;
        }

        if (conf->getGroupList()->contains(groupName))
        {
            listWidget->editItem(listWidget->item(row));
            QMessageBox * box = new QMessageBox(this);
            box->setWindowTitle("警告：");
            box->setText("组名已经存在了，请修改成其他名字");
            box->show();
            return;
        }

        if (groupName.size() == 0 || groupName.count() > 20)
        {
            listWidget->editItem(listWidget->item(row));
            QMessageBox * box = new QMessageBox(this);
            box->setWindowTitle("警告：");
            box->setText("分组名称不能为空或者长度大于20，请重新修改！");
            box->show();
            return;
        }

        Logger::instance().log("Group name: " + groupName.toStdString(), Logger::kLogLevelDebug);
        if (row == conf->getGroupList()->count())
        {
            conf->addGroup(groupName);
        }
        else
        {
            conf->editGroupName(row, groupName);
            emit(groupNameChanged(row));
        }

        conf->setDirty();
    });

    layout->addWidget(listWidget);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);
    setFixedSize(300, 200);
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - this->rect().center());
    setWindowTitle("分组列表");
    // setWindowFlag(Qt::WindowStaysOnTopHint, true);
    // setWindowFlag(Qt::CustomizeWindowHint, true);
    // setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
    Qt::WindowFlags f = windowFlags();
    f |= Qt::CustomizeWindowHint;
    f &= ~Qt::WindowMinMaxButtonsHint;
    setWindowFlags(f);
}

ListGroup::~ListGroup()
{
}