#ifndef APP_H
#define APP_H

#include "player.h"
#include "conf.h"

class App
{
public:
    Player *player;
    Conf *conf;

public:
    App();
    ~App();
    void run();

private:

};

#endif // APP_H
