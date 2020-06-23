#include "app.h"

App::App()
{
    conf = Conf::instance();
    player = new Player();
    player->bind();
}

App::~App()
{
    if (conf->isDirty())
    {
        conf->save();
    }
    delete player;
}

void App::run()
{
    player->run();
}