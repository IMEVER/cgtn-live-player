#ifndef APP_H
#define APP_H

#include "about.h"
#include "player.h"


class App
{
private:
    std::vector<Item> tvVector;
    bool loadCCTV = false;

public:
    About *about;
    Player *player;

public:
    App();
    ~App();
    void run();
    void showAbout();

private:
    void init();
    void loadTvVector();
};

#endif // APP_H
