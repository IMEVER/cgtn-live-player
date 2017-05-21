#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QIcon>
#include <QLabel>

class AboutDialog : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(QString productName READ productName WRITE setProductName)
    Q_PROPERTY(QString productLink READ productLink WRITE setProductLink)
    Q_PROPERTY(QString websiteName READ websiteName WRITE setWebsiteName)
    Q_PROPERTY(QString websiteLink READ websiteLink WRITE setWebsiteLink)

private:
   QString _productName;
   QString _productLink;
   QString _websiteName;
   QString _websiteLink;

   QLabel *productIcomLabel;
   QLabel *productNameLabel;
   QLabel *websiteNameLabel;
   QLabel *versionLabel;
   QLabel *descriptionLabel;

   void updateProductWebsite();
   void updateWebsite();

public:
    explicit AboutDialog(QWidget *parent=0);
    ~AboutDialog();

    void setProductIcon(const QIcon &icon);
    QString productName() const;
    void setProductName(const QString &name);
    QString productLink() const;
    void setProductLink(const QString &link);
    void setWebsiteLink(const QString &link);
    QString websiteLink() const;
    void setWebsiteName(const QString &websiteName);
    QString websiteName() const;
    void setVersion(const QString &version);
    void setDescription(const QString &description);
};

#endif // ABOUTDIALOG_H
