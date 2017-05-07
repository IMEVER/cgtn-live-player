#include "app.h"

App::App()
{
    about = new About;
    player = new Player();
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
