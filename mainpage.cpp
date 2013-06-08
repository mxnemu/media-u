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
        nw::JsonWriter jw;
        jw.describeArray("lists", "", airingShows.length());
        /*
        for (int i=0; jw.enterNextElement(i); ++i) {
            std::string name = show->name().toStdString();
            TvShow* show = airingShows.at(i);
            jw.describe("name", name);
        }
        */
        jw.close();
        //nw::Tag* jsonMotherTag = jw.
        Server::simpleWrite(resp, 200, "{\"lists\":\"[]\"}");
        qDebug() << "resp on lists";
        return true;
    } else if (req->path().startsWith("/api/page/background")) {
        Server::simpleWrite(resp, 200, QString("{\"image\":\"%1\"}").arg(library.randomWallpaperPath()));
        qDebug() << "resp on lists";
        return true;
    }
    return false;
}
