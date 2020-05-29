#ifndef LISTGROUP_H
#define LISTGROUP_H

#include <QMainWindow>
#include <QListWidget>

class ListGroup: public QMainWindow
{
    Q_OBJECT
private:
    QListWidget *listWidget;
public:
    ListGroup(QWidget *parent);
    ~ListGroup();

signals:
    void groupNameChanged(int index);
};


#endif