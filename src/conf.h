#ifndef CONF_H
#define CONF_H

#include <QString>

struct Item
{
    QString title;
    QString url;

    public:
    
    Item(QString title, QString url)
    {
        this->title = title;
        this->url = url;
    }
};

struct Group
{
    QString name;
    QList<Item *> *tvs;

    public:
    Group(QString name, QList<Item *> *tvs)
    {
        this->name = name;
        this->tvs = tvs;
    }
};

struct JsonConf
{
    QString version;
    QString name;
    QString date;
    QString author;
    QList<Group *> *groups;
};

// extern JsonConf conf;

class Conf
{
private:
    static Conf *pInstance;
    JsonConf conf;
    QStringList *groupList;
    bool dirty = false;

public:
    static Conf *instance();
    JsonConf getJsonConf();
    QStringList *getGroupList();
    void addGroup(QString groupName);
    void editGroupName(int index, QString groupName);
    int updateTvGroup(int tvIndex, QString toGroup);
    void moveUp(int index);
    void moveDown(int index);
    void setDirty();
    bool isDirty();
    void save();
    
    virtual ~Conf();

private:
    Conf(/* args */);

    void addGroup(QString groupName, QList<Item *> *itemList);
    void init();
    void loadTvVector();
};

#endif // CONF_H
