#ifndef APP_H
#define APP_H

#include "about.h"
#include "player.h"


class App
{
public:
    About *about;
    Player *player;

public:
    App();
    ~App();
    void run();
    void showAbout();
};

#endif // APP_H
