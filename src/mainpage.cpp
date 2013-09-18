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

    QString backgroundPath = library.randomWallpaperPath();
    //this->setStyleSheet(QString("background-color:black;background-image: url('%1');").arg(backgroundPath));

    // TODO update ui
    //this->ui->currentlyAiringShows = new TvShowListWidget();
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
