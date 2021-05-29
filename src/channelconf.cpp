#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include "logger.h"

#include "channelconf.h"

const QString jsonFile = "tv.json";
ChannelConf *ChannelConf::pInstance = nullptr;

ChannelConf *ChannelConf::instance()
{
    if (pInstance == nullptr)
    {
        pInstance = new ChannelConf;
    }

    return pInstance;
}

ChannelConf::ChannelConf(/* args */)
{
    groupList = new QStringList;
    conf.groups = new QList<Group *>();
    init();
    loadTvVector();
}

ChannelConf::~ChannelConf()
{
    if (dirty)
    {
        save();
    }

    delete groupList;
}

JsonConf ChannelConf::getJsonConf()
{
    return conf;
}

QStringList *ChannelConf::getGroupList()
{
    return groupList;
}

void ChannelConf::setDirty()
{
    dirty = true;
}

bool ChannelConf::isDirty()
{
    return dirty;
}

void ChannelConf::addGroup(QString groupName)
{
    QList<Channel *> *itemList = new QList<Channel *>();
    addGroup(groupName, itemList);
}

void ChannelConf::addGroup(QString groupName, QList<Channel *> *itemList)
{
    groupList->append(groupName);
    Group *group = new Group(groupName, itemList);
    conf.groups->append(group);
}

void ChannelConf::editGroupName(int index, QString groupName)
{
    groupList->replace(index, groupName);
    conf.groups->at(index)->name = groupName;
    setDirty();
}

int ChannelConf::updateTvGroup(int tvIndex, QString toGroup)
{
    int toIndex = -1;
    int toGroupIndex = groupList->indexOf(toGroup);
    if (toGroupIndex > -1)
    {
        int groupIndex = 0;
        int index = tvIndex;
        foreach(Group *group, *conf.groups)
        {
            if (index >= group->tvs->count())
            {
                index -= group->tvs->count();
                groupIndex++;
            }
            else
            {
                break;
            }
        }
        if (groupIndex < groupList->count())
        {
            Group *group = conf.groups->at(groupIndex);
            if (index < group->tvs->count())
            {
                Channel *item = group->tvs->takeAt(index);
                conf.groups->at(toGroupIndex)->tvs->append(item);
                for (; toGroupIndex >= 0; toGroupIndex--)
                {
                    toIndex += conf.groups->at(toGroupIndex)->tvs->count();
                }

                setDirty();
            }
        }
    }

    return toIndex;
}

void ChannelConf::moveUp(int index)
{
    int groupIndex = 0;
    foreach(Group *group, *conf.groups)
    {
        if (index >= group->tvs->count())
        {
            index -= group->tvs->count();
            groupIndex++;
        }
        else
        {
            break;
        }
    }

    if (groupIndex < conf.groups->count() && index >0)
    {
        Group *group = conf.groups->at(groupIndex);
        if (group->tvs->count() > index)
        {
            group->tvs->insert(index -1, group->tvs->takeAt(index));
            setDirty();
        }
    }
}

void ChannelConf::moveDown(int index)
{
    int groupIndex = 0;
    foreach(Group *group, *conf.groups)
    {
        if (index >= group->tvs->count())
        {
            index -= group->tvs->count();
            groupIndex++;
        }
        else
        {
            break;
        }
    }

    if (groupIndex < conf.groups->count())
    {
        Group *group = conf.groups->at(groupIndex);
        if (group->tvs->count() -1 > index)
        {
            group->tvs->insert(index+1, group->tvs->takeAt(index));
            setDirty();
        }
    }
}

void ChannelConf::init()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    QFile file(configPath + "/" + jsonFile);
    Logger::instance().log("Local config path: " + configPath.toStdString() + ", name: " + file.fileName().toStdString());

    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    if (!file.exists())
    {
        QFile conf(":/resource/tv.json");
        conf.copy(file.fileName());
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
    }
}

void ChannelConf::loadTvVector()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + jsonFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray bytearray = file.readAll();
    file.close();
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(bytearray, &error);
    if (error.error != QJsonParseError::NoError)
    {
        Logger::instance().log("Error occurs when parse json file, err msg: " + error.errorString().toStdString(), Logger::kLogLevelError);

        QFile file(":/resource/tv.json");
        json = QJsonDocument::fromJson(file.readAll());
        file.close();
    }
    Logger::instance().log(json.toJson().toStdString(), Logger::kLogLevelDebug);
    QJsonObject data = json.object();

    conf.version = data.value("version").toString();
    conf.name = data.value("name").toString();
    conf.date = data.value("date").toString();
    conf.author = data.value("author").toString();

    QJsonArray groups = data.value("tvs").toArray();

    int tvCount = 0;
    foreach (const QJsonValue group, groups)
    {
        QJsonArray tvsArray = group.toObject().value("list").toArray();

        tvCount += tvsArray.size();
        QList<Channel *> *tvs = new QList<Channel *>;

        foreach (const QJsonValue &v, tvsArray)
        {
            QJsonObject item = v.toObject();
            if (item.contains("ignore") && item.value("ignore").toBool())
            {
                continue;
            }
            else
            {
                tvs->append(new Channel(item.value("title").toString(), item.value("url").toString()));
            }
        }
        addGroup(group.toObject().value("group").toString(), tvs);
        Logger::instance().log("province: " + group.toObject().value("group").toString().toStdString() + ", tv count: " + QString::number(tvsArray.size()).toStdString());
    }
    Logger::instance().log("Tvs count: " + std::to_string(tvCount));
}

void ChannelConf::save()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + jsonFile);
    // QFile file("/tmp/" + jsonFile);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QStringList txt;
    txt.append("{");
    txt.append("\t\"version\": \"" + conf.version + "\",");
    txt.append("\t\"name\": \"" + conf.name + "\",");
    txt.append("\t\"author\": \"" + conf.author + "\",");
    txt.append("\t\"date\": \"" + conf.date + "\",");
    txt.append("\t\"tvs\": [");

    int groupIndex = conf.groups->length() -1;
    foreach(Group *group, *conf.groups)
    {
        txt.append("\t\t{");
        txt.append("\t\t\t\"group\": \"" + group->name + "\",");
        txt.append("\t\t\t\"list\": [");

        int tvIndex = group->tvs->length() - 1;
        foreach(Channel *item, *group->tvs)
        {
            txt.append("\t\t\t\t{");
            txt.append("\t\t\t\t\t\"title\": \"" + item->title + "\",");
            txt.append("\t\t\t\t\t\"url\": \"" + item->url + "\"");
            txt.append(QString("\t\t\t\t}").append(tvIndex-- == 0 ? "" : ","));
        }

        txt.append("\t\t\t]");
        txt.append(QString("\t\t}").append(groupIndex-- == 0 ? "" : ","));
    }
    txt.append("\t]");
    txt.append("}");

    QByteArray array = txt.join("\n").toUtf8();

    file.write(array, array.length());
    file.flush();
    file.close();
}