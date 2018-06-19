#include <QApplication>
#include <singleapplication.h>
#include <qpa/qplatformintegrationfactory_p.h>
#include "app.h"
#include <QtGui>

#define DXCB_PLUGIN_KEY "dxcb"

int main(int argc, char *argv[])
{
    if (QPlatformIntegrationFactory::keys().contains(DXCB_PLUGIN_KEY)) {
         qputenv("QT_QPA_PLATFORM", DXCB_PLUGIN_KEY);
    }

//    QApplication a(argc, argv);
    SingleApplication a(argc, argv);
    a.setApplicationName("cgtn");
    App app;
    app.run();


//    QObject::connect(&a, &SingleApplication::instanceStarted, &app, &App::showAbout);

    return a.exec();
}
