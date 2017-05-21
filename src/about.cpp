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
    pixmap.load(QString(":/resource/cgtn-live-player.png"));

    about = new AboutDialog;
    about->setAttribute(Qt::WA_ShowModal, true);
    about->setWindowTitle("About");
    QIcon icon;
    icon.addPixmap(pixmap, QIcon::Normal);

    about->setProductIcon(icon);
    about->setProductName("Cgtn live player");
    about->setProductLink("https://github.com/IMEVER/cgtn-live-player");
    about->setWebsiteLink("http://www.imever.me");
    about->setWebsiteName("IMEVER");
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
