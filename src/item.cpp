#include "item.h"

Item::Item(QString title, QString url)
{
        this->title = title;
        this->url = url;
}

QString Item::getTitle()
{
    return title;
}
QString Item::getUrl()
{
    return url;
}
