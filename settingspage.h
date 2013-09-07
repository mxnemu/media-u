#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include "library.h"
#include "page.h"

namespace Ui {
class SettingsPage;
}

class SettingsPage : public Page
{
    Q_OBJECT
    
public:
    explicit SettingsPage(Library& library, QWidget *parent = 0);
    ~SettingsPage();


    virtual bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);
    virtual void initFromQuery(const QString& initString);
    
private:
    Ui::SettingsPage *ui;
    Library& library;
};

#endif // SETTINGSPAGE_H
