#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include "logger.h"

#include "conf.h"

const QString jsonFile = "tv.json";
Conf *Conf::pInstance = nullptr;

Conf *Conf::instance()
{
    if (pInstance == nullptr)
    {
        pInstance = new Conf;
    }

    return pInstance;
}

Conf::Conf(/* args */)
{
    groupList = new QStringList;
    conf.groups = new QList<Group *>();
    init();
    loadTvVector();
}

Conf::~Conf()
{
    delete pInstance;
    delete groupList;
}

JsonConf Conf::getJsonConf()
{
    return conf;
}

QStringList *Conf::getGroupList()
{
    return groupList;
}

void Conf::setDirty()
{
    dirty = true;
}

bool Conf::isDirty()
{
    return dirty;
}

void Conf::addGroup(QString groupName)
{
    QList<Item *> *itemList = new QList<Item *>();
    addGroup(groupName, itemList);
}

void Conf::addGroup(QString groupName, QList<Item *> *itemList)
{
    groupList->append(groupName);
    Group *group = new Group(groupName, itemList);
    conf.groups->append(group);
}

void Conf::editGroupName(int index, QString groupName)
{
    groupList->replace(index, groupName);
    conf.groups->at(index)->name = groupName;
}

void Conf::init()
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

void Conf::loadTvVector()
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
        QList<Item *> *tvs = new QList<Item *>;

        foreach (const QJsonValue &v, tvsArray)
        {
            QJsonObject item = v.toObject();
            if (item.contains("ignore") && item.value("ignore").toBool())
            {
                continue;
            }
            else
            {
                tvs->append(new Item(item.value("title").toString(), item.value("url").toString()));
            }
        }
        addGroup(group.toObject().value("group").toString(), tvs);
        Logger::instance().log("province: " + group.toObject().value("group").toString().toStdString() + ", tv count: " + QString::number(tvsArray.size()).toStdString());
    }
    Logger::instance().log("Tvs count: " + std::to_string(tvCount));
}

void Conf::save()
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
        foreach(Item *item, *group->tvs)
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