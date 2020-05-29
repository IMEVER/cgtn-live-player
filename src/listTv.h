#ifndef LISTTV_H
#define LISTTV_H

#include <QMainWindow>
#include <QStandardItemModel>

class  ListTvWindow : public QMainWindow
{
    Q_OBJECT
private:
    QStandardItemModel *model;
public:
     explicit ListTvWindow(QWidget *parent=0);
    ~ ListTvWindow();

signals:
    void groupNameChanged(int row);
    void tvTitleChanged(int groupIndex, int tvIndex);
};

#endif //LISTTV_H