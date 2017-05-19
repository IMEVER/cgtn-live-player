#include <QApplication>
#include "app.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    App app;
    app.run();

    return a.exec();
}
