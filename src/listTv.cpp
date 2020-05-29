#include "listTv.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>
#include <QApplication>
#include <QDesktopWidget>
#include "conf.h"
#include "listGroup.h"
#include "logger.h"

ListTvWindow::ListTvWindow(QWidget *parent) : QMainWindow(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    QHBoxLayout *toolBar = new QHBoxLayout;
    toolBar->setAlignment(Qt::AlignLeft);
    QPushButton *editGroup = new QPushButton(this);
    editGroup->setText("修改分组");
    editGroup->setFixedWidth(100);
    connect(editGroup, &QPushButton::clicked, [=](bool checked) {
        Q_UNUSED(checked);
        ListGroup *listGroup = new ListGroup(this);
        listGroup->show();

        connect(listGroup, &ListGroup::groupNameChanged, [=](const int index) {
            QList<Group *> *list = Conf::instance()->getJsonConf().groups;
            int startIndex = 0, endIndex = 0, i = 0;
            while (i < index)
            {
                startIndex += list->at(i)->tvs->count();
                i++;
            }
            endIndex = startIndex + list->at(index)->tvs->count();

            for (; startIndex < endIndex; startIndex++)
            {
                model->item(startIndex, 1)->setText(list->at(index)->name);
            }

            emit(groupNameChanged(index));
        });
    });
    toolBar->addWidget(editGroup);
    mainLayout->addLayout(toolBar);

    QTableView *tableView = new QTableView;
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    model = new QStandardItemModel;
    model->setColumnCount(3);
    model->setHeaderData(0, Qt::Horizontal, QString("名称"));
    model->setHeaderData(1, Qt::Horizontal, "分组");
    model->setHeaderData(2, Qt::Horizontal, "网址");

    int groupIndex = 0, row = 0;
    foreach (const Group *group, *Conf::instance()->getJsonConf().groups)
    {
        int tvIndex = 0;
        foreach (const Item *item, *group->tvs)
        {
            QStandardItem *titleItem = new QStandardItem(item->title);
            // QVector<int> indexes(2);
            // indexes[0] = groupIndex;
            // indexes[1] = tvIndex;
            // titleItem->setData(QVariant::fromValue(indexes));
            model->setItem(row, 0, titleItem);
            model->setItem(row, 1, new QStandardItem(group->name));
            model->setItem(row, 2, new QStandardItem(item->url));

            tvIndex++;
            row++;
        }
        groupIndex++;
    }

    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

    tableView->setColumnWidth(0, 180);
    tableView->setColumnWidth(1, 90);
    connect(tableView, &QTableView::closePersistentEditor, [=](const QModelIndex &index) {
        Logger::instance().log("Current name: " + model->itemFromIndex(index)->text().toStdString());
    });

    connect(tableView->itemDelegate(), &QAbstractItemDelegate::closeEditor, [=](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {
        Q_UNUSED(hint);
        QString content = reinterpret_cast<QLineEdit *>(editor)->text();
        QModelIndex indexModel = tableView->currentIndex();
        Conf *conf = Conf::instance();

        int indexTv = indexModel.row();
        int indexGroup = 0;
        foreach (Group *group, *conf->getJsonConf().groups)
        {
            if (group->tvs->count() < indexTv)
            {
                indexTv -= group->tvs->count();
                indexGroup++;
            }
            else
            {
                break;
            }
        }

        Item *tv = conf->getJsonConf().groups->at(indexGroup)->tvs->at(indexTv);
        switch (indexModel.column())
        {
        case 0:
            if (indexModel.column() == 0 && content.compare(tv->title) == 0)
            {
                return;
            }
            Logger::instance().log("Old title: " + tv->title.toStdString(), Logger::kLogLevelDebug);
            Logger::instance().log("New title: " + content.toStdString(), Logger::kLogLevelDebug);

            tv->title = content;
            emit(tvTitleChanged(indexGroup, indexTv));
            conf->setDirty();
            break;
        case 2:
            if (indexModel.column() == 2 && content.compare(tv->url) == 0)
            {
                return;
            }
            tv->url = content;
            conf->setDirty();
            break;
        }
    });

    mainLayout->addWidget(tableView);

    setFixedSize(800, 500);
    move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    setWindowTitle("电视频道列表");
    Conf::instance()->save();
}

ListTvWindow::~ListTvWindow()
{
    delete model;
}