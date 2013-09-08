#include "tvshowpage.h"
#include "ui_tvshowpage.h"
#include "server.h"

TvShowPage::TvShowPage(Library& library, QWidget *parent) :
    Page(parent),
    ui(new Ui::TvShowPage),
    library(library)
{
    ui->setupUi(this);
}

TvShowPage::~TvShowPage()
{
    delete ui;
}

void TvShowPage::initFromQuery(const QString &initString) {
    TvShow* show = library.existingTvShow(initString);
    if (show) {
        this->setTvShow(show);
    }
}

void TvShowPage::setTvShow(TvShow* show) {
    this->tvShow = show;
    ui->title->setText(show->name());
    if (show->coverPath(library.getDirectory()).length() > 0) {
        QPixmap pix(show->coverPath(library.getDirectory()));
        ui->cover->setPixmap(pix);
    }
    ui->episodes->setText(QString("%1/%2/%3").arg(0).arg(show->episodesDownloaded()).arg(show->getTotalEpisodes()));
    ui->status->setText(show->getAiringStatus());
    ui->startDate->setText(show->getStartDate().toString("yyyy-MM-dd"));
    ui->endDate->setText(show->getEndDate().toString("yyyy-MM-dd"));
    ui->medium->setText(show->getShowType());
    ui->synopsis->setText(show->getSynopsis());

    QString backgroundWallpaper = show->randomWallpaper(library.getDirectory());
    if (!backgroundWallpaper.isNull()) {
        dynamic_cast<MainBackgroundWidget*>(this->parentWidget())->setBackground(backgroundWallpaper);
    } else {
        dynamic_cast<MainBackgroundWidget*>(this->parentWidget())->setBackground(QString());
    }
}

bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path() == "/api/page/showDetails") {
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.setState("detailed", true);
        tvShow->write(jw);
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data());
        return true;
    }
    return false;
}
