#include <QApplication>
#include <singleapplication.h>
#include "app.h"
#include <QtGui>

#define DXCB_PLUGIN_KEY "dxcb"

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
    SingleApplication a(argc, argv);
    a.setApplicationName("cgtn");
    App app;
    app.run();

//    QObject::connect(&a, &SingleApplication::instanceStarted, &app, &App::showAbout);

    return a.exec();
}
