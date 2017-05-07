#ifndef ABOUT_H
#define ABOUT_H

#include <DAboutDialog>

DWIDGET_USE_NAMESPACE

class About : public QObject
{
    Q_OBJECT

private:
    DAboutDialog *about;

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
