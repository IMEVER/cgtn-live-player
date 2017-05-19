#ifndef ABOUT_H
#define ABOUT_H

#include "aboutdialog.h"

class About : public QObject
{
    Q_OBJECT

private:
    AboutDialog *about;

public:
    About();
    ~About();
    void show();


private:
    void init();

public slots:
    void triggered();
};

#endif // ABOUT_H
