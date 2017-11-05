#include "app.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

App::App()
{
    init();
    loadTvVector();

    about = new About;
    player = new Player(tvVector, loadCCTV);
    player->bind(about);
}

App::~App()
{
    delete about;
    delete player;
}

void App::run()
{
    player->run();
}

void App::showAbout()
{
    about->show();
}

void App::init()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    QFile file(configPath +  "/tv.json");
    qDebug()<<"Local config path: "<<configPath<<", name: "<<file.fileName();

    if(!dir.exists())
    {
        dir.mkpath(".");
    }

    if(!file.exists())
    {
        QFile conf(":/resource/tv.json");
        conf.copy(file.fileName());
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
    }
}

void App::loadTvVector()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/tv.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray bytearray = file.readAll();
    file.close();
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(bytearray, &error);
    if(error.error != QJsonParseError::NoError)
    {
        qDebug()<<"Error occurs when parse json file, err msg: "<<error.errorString();

        QFile file(":/resource/tv.json");
        json = QJsonDocument::fromJson(file.readAll());
        file.close();
    }
        qDebug()<<json.isObject();
        QJsonObject data = json.object();

        QJsonArray tvsArray = data.value("tvs").toArray();
        foreach(const QJsonValue &v, tvsArray)
        {
            QJsonObject item = v.toObject();
            if(item.contains("ignore") && item.value("ignore").toBool()) {
                continue;
            } else if(item.contains("separater")){
               if(tvVector.size() > 0)
                   tvVector.at(tvVector.size() - 1).setSeparate(true);
           } else {
               tvVector.push_back(Item(item.value("title").toString(), item.value("url").toString()));
           }
        }

        if(data.value("cctv").toBool())
        {
            loadCCTV = true;
        }
    qDebug()<<"Tvs count: "<<tvVector.size();
}
