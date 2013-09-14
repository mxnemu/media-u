#include "settingspage.h"
#include "ui_settingspage.h"
#include <QFileDialog>
#include "searchdirectory.h"

SettingsPage::SettingsPage(Library &library, QWidget *parent) :
    Page(parent),
    ui(new Ui::SettingsPage),
    library(library)
{
    ui->setupUi(this);

    const QList<SearchDirectory>& directories = library.getSearchDirectories();
    for (int i=0; i < directories.length(); ++i) {
        const SearchDirectory& directory = directories.at(i);
        new QTreeWidgetItem(this->ui->searchDirectories, QStringList() <<
            directory.dir.path() <<
            (directory.enabled ? "enabled" : "disabled")
        );
    }
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

void SettingsPage::on_addButton_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Add Searchpath", QDir::rootPath());
    if (!dir.isNull()) {
        library.addSearchDirectory(SearchDirectory(QDir(dir), true));
        //this->ui->searchDirectories->addTopLevelItem();
        new QTreeWidgetItem(this->ui->searchDirectories, QStringList() << dir << "enabled");
    }
}

void SettingsPage::on_toggleDisableButton_clicked() {
    QList<QTreeWidgetItem*> items = this->ui->searchDirectories->selectedItems();
    for (int i=0; i < items.length(); ++i) {
        SearchDirectory* directory = library.getSearchDirectory(items.at(i)->text(0));
        if (directory) {
            directory->enabled = !directory->enabled;
            items[i]->setText(1, directory->enabled ? "enabled" : "disabled");
        }
    }
}

void SettingsPage::on_removeButton_clicked() {
    QList<QTreeWidgetItem*> items = this->ui->searchDirectories->selectedItems();
    for (int i=0; i < items.length(); ++i) {
        QTreeWidgetItem* item = items[i];
        if (library.removeSearchDirectory(items.at(i)->text(0))) {
            ui->searchDirectories->takeTopLevelItem(ui->searchDirectories->indexOfTopLevelItem(item));
            delete item;
        }
    }
}
