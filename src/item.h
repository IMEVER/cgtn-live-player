#ifndef ITEM_H
#define ITEM_H

#include <QString>

class Item
{
private:
    QString url;
    QString title;

public:
    Item(QString title, QString url);
    QString getTitle();
    QString getUrl();
};

#endif // ITEM_H
