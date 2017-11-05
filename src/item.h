#ifndef ITEM_H
#define ITEM_H

#include <QString>

class Item
{
private:
    QString url;
    QString title;
    bool needSeparate = false;

public:
    Item(QString title, QString url);
    QString getTitle();
    QString getUrl();
    void setSeparate(bool separate);
    bool isNeedSeparate();
};

#endif // ITEM_H
