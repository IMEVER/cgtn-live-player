#include "conf.h"

#include <QStandardPaths>

Conf *Conf::pInstance = nullptr;

Conf::Conf(/* args */) : dirty(false)
{
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/setting.ini", QSettings::IniFormat);
}

Conf::~Conf()
{
    if(dirty)
        settings->sync();
    delete settings;
}

Conf *Conf::instance()
{
    if (pInstance == nullptr)
    {
        pInstance = new Conf;
    }
    return pInstance;
}

bool Conf::isSidebarShow()
{
    return settings->value("side/isShow", true).toBool();
}

void Conf::setSidebarShow(bool show)
{
    settings->setValue("side/isShow", show);
    setDirty();
}

int Conf::volume()
{
    int volume = settings->value("volume", 50).toInt();
    if (volume > 100)
    {
        volume = 100;
    } else if(volume < 0)
    {
        volume = 0;
    }
    return volume;
}

void Conf::setVolume(int volume)
{
    if (volume > 100)
    {
        volume = 100;
    } else if(volume < 0)
    {
        volume = 0;
    }
    settings->setValue("volume", volume);
    setDirty();
}