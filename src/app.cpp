#include "app.h"
#include "conf.h"
#include "channelconf.h"

App::App()
{
    player = new Player();
    player->bind();
}

App::~App()
{
    delete ChannelConf::instance();
    delete player;
    delete Conf::instance();
}

void App::run()
{
    player->run();
}