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
    connect(tvShow, SIGNAL(watchCountChanged(int,int)), this, SLOT(updateWatched(int,int)));

    ui->title->setText(show->name());
    if (show->coverPath(library.getDirectory()).length() > 0) {
        QPixmap pix(show->coverPath(library.getDirectory()));
        ui->cover->setPixmap(pix);
    }
    ui->status->setText(show->getAiringStatus());
    ui->startDate->setText(show->getStartDate().toString("yyyy-MM-dd"));
    ui->endDate->setText(show->getEndDate().toString("yyyy-MM-dd"));
    ui->medium->setText(show->getShowType());
    ui->synopsis->setText(show->getSynopsis());

    this->updateWatched(0,0);

    QString backgroundWallpaper = show->randomWallpaper(library.getDirectory());
    if (!backgroundWallpaper.isNull()) {
        dynamic_cast<MainBackgroundWidget*>(this->parentWidget())->setBackground(backgroundWallpaper);
    } else {
        dynamic_cast<MainBackgroundWidget*>(this->parentWidget())->setBackground(QString());
    }
}

void TvShowPage::updateWatched(int,  int) {
    ui->episodes->setText(QString("%1/%2/%3").arg(
        QString::number(tvShow->getWatchedEpisodes()),
        QString::number(tvShow->episodesDownloaded()),
        QString::number(tvShow->getTotalEpisodes())
    ));
}

bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path() == "/api/page/showDetails") {
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.setState("detailed", true);
        tvShow->write(jw);
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
        return true;
    }
    return false;
}
