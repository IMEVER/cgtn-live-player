#include "aboutdialog.h"

#include <QWidget>
#include <QVBoxLayout>

const QString websiteLinkTemplate = "<a href='%1' style='text-decoration: none; font-size:13px; color: #004EE5;'>%2</a>";

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout();


    productIcomLabel = new QLabel();
    vboxLayout->addWidget(productIcomLabel, 0, Qt::AlignCenter);

    productNameLabel = new QLabel();
    productNameLabel->setTextFormat(Qt::RichText);
    productNameLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    productNameLabel->setOpenExternalLinks(true);
    productNameLabel->setStyleSheet("color: blue");
    vboxLayout->addWidget(productNameLabel, 0, Qt::AlignCenter);

    websiteNameLabel = new QLabel();
    websiteNameLabel->setTextFormat(Qt::RichText);
    websiteNameLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    websiteNameLabel->setOpenExternalLinks(true);
    vboxLayout->addWidget(websiteNameLabel, 0, Qt::AlignCenter);

    versionLabel = new QLabel();
    versionLabel->setText("1.0.0");
    versionLabel->setStyleSheet("color: black");
    vboxLayout->addWidget(versionLabel, 0, Qt::AlignCenter);

    descriptionLabel = new QLabel();
    descriptionLabel->setText("description");
    descriptionLabel->setStyleSheet("color: black");
    vboxLayout->addWidget(descriptionLabel, 0, Qt::AlignCenter);

    setLayout(vboxLayout);
    setStyleSheet("background-color: white");
    setFixedWidth(400);
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    // setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::setProductIcon(const QIcon &icon)
{
    productIcomLabel->setPixmap(icon.pixmap(QSize(96, 96)));
}

QString AboutDialog::productName() const
{
    return _productName;
}

void AboutDialog::setProductName(const QString &name)
{
    _productName = name;
    updateProductWebsite();
}

QString AboutDialog::productLink() const
{
    return _productLink;
}

void AboutDialog::setProductLink(const QString &link)
{
    _productLink = link;
    updateProductWebsite();
}

void AboutDialog::updateProductWebsite()
{
    productNameLabel->setText(QString(websiteLinkTemplate).arg(productLink()).arg(productName()));
}

QString AboutDialog::websiteName() const
{
    return _websiteName;
}

QString AboutDialog::websiteLink() const
{
    return _websiteLink;
}

void AboutDialog::updateWebsite()
{
    websiteNameLabel->setText(QString(websiteLinkTemplate).arg(websiteLink()).arg(websiteName()));
}

void AboutDialog::setWebsiteLink(const QString &link)
{
    _websiteLink = link;
    updateWebsite();
}

void AboutDialog::setWebsiteName(const QString &websiteName)
{
    _websiteName = websiteName;
    updateWebsite();
}

void AboutDialog::setVersion(const QString &version)
{
    versionLabel->setText(version);
}

void AboutDialog::setDescription(const QString &description)
{
    descriptionLabel->setText(description);
}
