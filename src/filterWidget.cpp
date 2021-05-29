#include "filterWidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include "channelconf.h"

FilterWidget::FilterWidget(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

        //Add filter search
    QLineEdit *search = new QLineEdit(this);
    // search->clearFocus();
    // search->setFixedWidth(160);
    search->setPlaceholderText("请输入搜索关键字");
    connect(search, &QLineEdit::textChanged, [=](const QString txt) {
        filterModel->setFilterFixedString(txt);
        treeMenu->expandAll();
    });

    layout->addWidget(search);

    //Add tree list menu
    treeMenu = new QTreeView(this);
    treeMenu->setHeaderHidden(true);
    // treeMenu->setFixedWidth(160);
    // QPalette palette = treeMenu->palette();
    // palette.setColor(QPalette::Background, qRgba(0, 0, 0, 100));
    // treeMenu->setPalette(palette);

    model = new QStandardItemModel(this);
    model->setColumnCount(1);
    QStandardItem *root = model->invisibleRootItem();

    int groupIndex = 0;
    foreach (const Group *group, *ChannelConf::instance()->getJsonConf().groups)
    {
        int tvIndex = 0;
        QStandardItem *groupItem = new QStandardItem(group->name + "(" + QString::number(group->tvs->size()) + ")");
        foreach (const Channel *item, *group->tvs)
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

    filterModel = new QSortFilterProxyModel(this);
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
            emit tvItemDoubleClicked(ss[0], ss[1]);
        }
    });

    layout->addWidget(treeMenu);
}

FilterWidget::~FilterWidget()
{
}

void FilterWidget::updateGroupName(int index)
{
    model->item(index, 0)->setText(ChannelConf::instance()->getGroupList()->at(index));
}

void FilterWidget::updateTvTitle(int groupIndex, int tvIndex)
{
    model->item(groupIndex, 0)->child(tvIndex, 0)->setText(ChannelConf::instance()->getJsonConf().groups->at(groupIndex)->tvs->at(tvIndex)->title);
}