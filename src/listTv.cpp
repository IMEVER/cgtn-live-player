#include "listTv.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>

#include <QItemDelegate>
#include <QComboBox>

#include "channelconf.h"
#include "listGroup.h"
#include "logger.h"

class GroupDelegate : public QItemDelegate
{
private:
    QStringList *groupList;
    QTableView *tableView;

public:
    // GroupDelegate(QObject *parent=0) : QItemDelegate(parent) {}
    GroupDelegate(QTableView *tableView, QStringList *groupList, QObject *parent = 0) : QItemDelegate(parent)
    {
        this->tableView = tableView;
        this->groupList = groupList;
    }
    ~GroupDelegate()
    {
        //delete groupList;
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        QComboBox *editor = new QComboBox(parent);
        foreach (QString groupName, *groupList)
        {
            editor->addItem(groupName);
        }
        return editor;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QString text = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        int tindex = comboBox->findText(text);
        comboBox->setCurrentIndex(tindex);
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
    {
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        QString text = comboBox->currentText();
        model->setData(index, text, Qt::EditRole);
        int toIndex = ChannelConf::instance()->updateTvGroup(index.row(), text);
        if (toIndex != -1)
        {
            // model->moveRow(index.parent(), index.row(), model->index(toIndex, 0).parent(), toIndex);

            QStandardItemModel *standardModel = static_cast<QStandardItemModel *>(model);
            QList<QStandardItem *> list = standardModel->takeRow(index.row());
            standardModel->insertRow(toIndex, list);
            tableView->setCurrentIndex(standardModel->index(toIndex, 1));
            tableView->selectRow(toIndex);
        }
    }
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }
};

ListTvWindow::ListTvWindow(QWidget *parent) : QMainWindow(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QHBoxLayout *toolBar = new QHBoxLayout(this);
    toolBar->setAlignment(Qt::AlignCenter);
    QPushButton *editGroup = new QPushButton(this);
    editGroup->setText("修改分组");
    editGroup->setFixedWidth(100);
    connect(editGroup, &QPushButton::clicked, [=](bool checked) {
        Q_UNUSED(checked);
        ListGroup *listGroup = new ListGroup(this);
        listGroup->show();

        connect(listGroup, &ListGroup::groupNameChanged, [=](const int index) {
            QList<Group *> *list = ChannelConf::instance()->getJsonConf().groups;
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

    QPushButton *moveUp = new QPushButton(this);
    moveUp->setText("向上移");
    moveUp->setFixedWidth(80);
    connect(moveUp, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked);
        int row = tableView->currentIndex().row();
        if (row > 0)
        {
            if (model->data(model->index(row -1, 1)).toString().compare(model->data(model->index(row, 1)).toString()) == 0)
            {
                model->insertRow(row - 1, model->takeRow(row));
                tableView->setCurrentIndex(model->index(row -1, 0));
                tableView->selectRow(row -1);
                ChannelConf::instance()->moveUp(row);
            }
        }
    });
    toolBar->addWidget(moveUp);

    QPushButton *moveDown = new QPushButton(this);
    moveDown->setText("向下移");
    moveDown->setFixedWidth(80);
    connect(moveDown, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked);
        int row = tableView->currentIndex().row();
        if (row < model->rowCount() - 1)
        {
            if (model->data(model->index(row, 1)).toString().compare(model->data(model->index(row+1, 1)).toString()) == 0)
            {
                model->insertRow(row + 1, model->takeRow(row));
                tableView->setCurrentIndex(model->index(row + 1, 0));
                tableView->selectRow(row + 1);
                ChannelConf::instance()->moveDown(row);
            }
        }
    });
    toolBar->addWidget(moveDown);

    mainLayout->addLayout(toolBar);

    tableView = new QTableView(this);
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    tableView->setItemDelegateForColumn(1, new GroupDelegate(tableView, ChannelConf::instance()->getGroupList()));

    model = new QStandardItemModel(this);
    model->setColumnCount(3);
    model->setHeaderData(0, Qt::Horizontal, QString("名称"));
    model->setHeaderData(1, Qt::Horizontal, "分组");
    model->setHeaderData(2, Qt::Horizontal, "网址");

    int groupIndex = 0, row = 0;
    foreach (const Group *group, *ChannelConf::instance()->getJsonConf().groups)
    {
        int tvIndex = 0;
        foreach (const Channel *item, *group->tvs)
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
        ChannelConf *conf = ChannelConf::instance();

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

        Channel *tv = conf->getJsonConf().groups->at(indexGroup)->tvs->at(indexTv);
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
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - this->rect().center());

    setWindowTitle("电视频道列表");
}

ListTvWindow::~ListTvWindow()
{
}