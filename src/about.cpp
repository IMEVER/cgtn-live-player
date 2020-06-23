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
    
    about->setWindowTitle("关于");

    QIcon icon;
    icon.addPixmap(pixmap, QIcon::Normal);

    about->setProductIcon(icon);
    about->setProductName("央视外语频道播放器");
    about->setProductLink("https://github.com/IMEVER/cgtn-live-player");
    about->setWebsiteLink("http://www.imever.me");
    about->setWebsiteName("IMEVER");
    about->setVersion("3.1.1");
    about->setDescription("央视外语频道播放器");
}

void About::show()
{
    about->show();
}

void About::triggered()
{
    show();
}
