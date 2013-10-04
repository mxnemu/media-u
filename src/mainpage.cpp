#include "mainpage.h"
#include "ui_mainpage.h"
#include "tvshowlistwidget.h"
#include "server.h"
#include "mainwindow.h"

MainPage::MainPage(Library& library, MainWindow* mainwindow, QWidget *parent) :
    Page(parent),
    ui(new Ui::MainPage),
    library(library),
    mainwindow(mainwindow)
{
    ui->setupUi(this);

    if (library.getSearchStatus() != Library::done) {
        connect(&library, SIGNAL(searchFinished()), this, SLOT(setRandomWallpaperAfterSearch()));
        // could this race even happen? I don't know
        if (library.getSearchStatus() == Library::done) {
            this->setRandomWallpaper();
        }
    } else {
        this->setRandomWallpaper();
    }

    this->airingShows = library.filter().airing();
    this->allShows = library.filter().all();
    this->ui->currentlyAiringShows->set(airingShows, QString("Airing Shows"));
    this->ui->allShows->set(allShows, QString("All Shows"));
    connect(&library, SIGNAL(showAdded(TvShow*)), this, SLOT(onShowAdded(TvShow*)));
}

MainPage::~MainPage()
{
    delete ui;
}

bool MainPage::handleApiRequest(QHttpRequest *, QHttpResponse *)
{
    return false;
}

void MainPage::on_settingsButton_clicked()
{
    mainwindow->setPage(PageFactory::settingsPageKey);
}

void MainPage::onShowAdded(TvShow *show) {
    if (show->isAiring()) {
        this->ui->currentlyAiringShows->add(show);
    }
    this->ui->allShows->add(show);
}

void MainPage::setRandomWallpaperAfterSearch() {
    QString path = library.filter().getRandomWallpaper();
    if (path.isNull()) {
        this->setRandomWallpaper(path);
    } else {
        connect(&library, SIGNAL(wallpaperDownloaded(QString)), this, SLOT(setRandomWallpaper(QString)));
    }
    disconnect(&library, SIGNAL(searchFinished()), this, SLOT(setRandomWallpaperAfterSearch()));
}

void MainPage::setRandomWallpaper(QString path) {
    MainBackgroundWidget* mbw = mainwindow->getCentralWidget();
    if (mbw) {
        if (path.isNull()) {
            path = library.filter().getRandomWallpaper();
        }
        mbw->setBackground(path);
    }
    disconnect(&library, SIGNAL(wallpaperDownloaded(QString)), this, SLOT(setRandomWallpaper(QString)));
}
