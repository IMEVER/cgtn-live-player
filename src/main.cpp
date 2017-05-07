#include <DApplication>
#include "app.h"

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    DApplication a(argc, argv);
    App app;

    app.run();

    a.setTheme("dark");

    return a.exec();
}
