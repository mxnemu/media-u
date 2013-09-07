#include "settingspage.h"
#include "ui_settingspage.h"

SettingsPage::SettingsPage(Library &library, QWidget *parent) :
    Page(parent),
    ui(new Ui::SettingsPage),
    library(library)
{
    ui->setupUi(this);
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

bool SettingsPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    return false;
}

void SettingsPage::initFromQuery(const QString &initString)
{
}
