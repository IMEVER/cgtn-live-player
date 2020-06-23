#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class FilterWidget : public QWidget
{
    Q_OBJECT
private:
    QWidget *parent;
    QTreeView *treeMenu;
    QStandardItemModel *model;
    QSortFilterProxyModel *filterModel;

public:
    FilterWidget(QWidget *parent = nullptr);
    ~FilterWidget();
    void updateGroupName(int index);
    void updateTvTitle(int groupIndex, int tvIndex);
signals:
    void tvItemDoubleClicked(int groupIndex, int tvIndex);
};


#endif //FILTERWIDGET_H