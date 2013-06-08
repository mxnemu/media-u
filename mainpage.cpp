#include "mainpage.h"
#include "ui_mainpage.h"
#include "tvshowlistwidget.h"
#include "server.h"

MainPage::MainPage(Library& library, QWidget *parent) :
    Page(parent),
    ui(new Ui::MainPage),
    library(library)
{
    ui->setupUi(this);

    QString backgroundPath = library.randomWallpaperPath();
    //this->setStyleSheet(QString("background-color:black;background-image: url('%1');").arg(backgroundPath));

    // TODO update ui
    //this->ui->currentlyAiringShows = new TvShowListWidget();
    this->airingShows = library.filter().all();
    dynamic_cast<TvShowListWidget*>(this->ui->currentlyAiringShows)->set(airingShows, QString("Airing Shows"));
}

MainPage::~MainPage()
{
    delete ui;
}

bool MainPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path().startsWith("/api/page/lists")) {
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.describeArray("lists", "", airingShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = airingShows.at(i);
            std::string name = show->name().toStdString();
            jw.describe("name", name);
        }
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data());
        qDebug() << "resp on lists" << ss.str().data();
        return true;
    } else if (req->path().startsWith("/api/page/background")) {
        Server::simpleWrite(resp, 200, QString("{\"image\":\"%1\"}").arg(library.randomWallpaperPath()));
        qDebug() << "resp on bgs";
        return true;
    }
    return false;
}
