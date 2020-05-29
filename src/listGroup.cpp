#include "listGroup.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include "conf.h"
#include "logger.h"

ListGroup::ListGroup(QWidget *parent) : QMainWindow(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *toolBar = new QHBoxLayout;
    toolBar->setAlignment(Qt::AlignLeft);
    QPushButton *addButton = new QPushButton;
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
    layout->addLayout(toolBar);

    listWidget = new QListWidget;
    listWidget->setSelectionBehavior(QTableWidget::SelectRows);
    listWidget->setSelectionMode(QTableWidget::SingleSelection);
    listWidget->setEditTriggers(QListWidget::DoubleClicked);

    foreach(QString group, *Conf::instance()->getGroupList())
    {
        listWidget->addItem(group);
    }

    for(int i=0, len=listWidget->count(); i<len; i++)
    {
        listWidget->item(i)->setFlags(listWidget->item(i)->flags() | Qt::ItemIsEditable);
    }

    connect(listWidget->itemDelegate(), &QAbstractItemDelegate::closeEditor, [=](QWidget *editor, QAbstractItemDelegate::EndEditHint hint){
        QString groupName = reinterpret_cast<QLineEdit *>(editor)->text();
        int row = listWidget->currentRow();
        Conf *conf = Conf::instance();

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

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    resize(300, 300);
}

ListGroup::~ListGroup()
{
    delete listWidget;
}