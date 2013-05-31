#include "mainpage.h"
#include "ui_mainpage.h"
#include "tvshowlistwidget.h"

MainPage::MainPage(Library& library, QWidget *parent) :
    Page(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);

    QString backgroundPath = library.randomWallpaperPath();
    //this->setStyleSheet(QString("background-color:black;background-image: url('%1');").arg(backgroundPath));

    // TODO update ui
    //this->ui->currentlyAiringShows = new TvShowListWidget();
    dynamic_cast<TvShowListWidget*>(this->ui->currentlyAiringShows)->set(library.airingShows(), QString("Airing Shows"));
}

MainPage::~MainPage()
{
    delete ui;
}

bool MainPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    return false;
}
