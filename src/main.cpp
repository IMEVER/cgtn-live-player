#include <QApplication>
#include <qpa/qplatformintegrationfactory_p.h>
#include "app.h"
#include <QtGui>

#define DXCB_PLUGIN_KEY "dxcb"

int main(int argc, char *argv[])
{
    if (QPlatformIntegrationFactory::keys().contains(DXCB_PLUGIN_KEY)) {
         qputenv("QT_QPA_PLATFORM", DXCB_PLUGIN_KEY);
    }

    QApplication a(argc, argv);
    App app;
    app.run();

    return a.exec();
}
