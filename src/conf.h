#ifndef CONF_H
#define CONF_H

#include <QSettings>

class Conf
{
private:
    QSettings *settings;
    bool dirty;
    static Conf *pInstance;

    Conf(/* args */);
    void setDirty(){dirty=true;}

public:
    static Conf *instance();
    ~Conf();

    bool isSidebarShow();
    void setSidebarShow(bool isShow);

    int volume();
    void setVolume(int volume);
};

#endif