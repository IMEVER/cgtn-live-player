#include "about.h"

About::About() : QObject()
{
    init();
}

About::~About()
{
    delete about;
}

void About::init()
{
    QPixmap pixmap;
    bool loaded = pixmap.load(QString(":/resource/cgtn-live-player.png"));
//    if(!loaded)
//        qDebug()<<"load image failed";

    about = new DAboutDialog;
    about->setWindowTitle("About");
    QIcon *icon = new QIcon();
    icon->addPixmap(pixmap, QIcon::Normal);
    about->setProductIcon(*icon);
    about->setProductName("Cgtn live player");
    about->setWebsiteLink("http://www.imever.me");
    about->setWebsiteName("IMEVER");
//    about->setCompanyLogo(pixmap);
    about->setAcknowledgementLink("http://www.imever.me");
    about->setVersion("1.0.0");
    about->setDescription("A cgtn live player for deepin user");
}

void About::show()
{
    about->show();
}

void About::triggered()
{
    show();
}
