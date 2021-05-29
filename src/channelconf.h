#ifndef CHANNELCONF_H
#define CHANNELCONF_H

#include <QString>

struct Channel
{
    QString title;
    QString url;

    public:

    Channel(QString title, QString url)
    {
        this->title = title;
        this->url = url;
    }
};

struct Group
{
    QString name;
    QList<Channel *> *tvs;

    public:
    Group(QString name, QList<Channel *> *tvs)
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

class ChannelConf
{
private:
    static ChannelConf *pInstance;
    JsonConf conf;
    QStringList *groupList;
    bool dirty = false;

public:
    static ChannelConf *instance();
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

    virtual ~ChannelConf();

private:
    ChannelConf(/* args */);

    void addGroup(QString groupName, QList<Channel *> *itemList);
    void init();
    void loadTvVector();
};

#endif // CONF_H
