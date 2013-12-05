#include "tvshowpage.h"
#include "ui_tvshowpage.h"
#include "server.h"

TvShowPage::TvShowPage(Library& library, QWidget *parent) :
    Page(parent),
    ui(new Ui::TvShowPage),
    library(library),
    tvShow(NULL)
{
    ui->setupUi(this);
}

TvShowPage::~TvShowPage()
{
    delete ui;
}

void TvShowPage::initFromQuery(const QString &initString) {
    TvShow* show = library.existingTvShow(initString);
    this->setTvShow(show);
}

void TvShowPage::setTvShow(TvShow* show) {
    this->tvShow = show;

    if (!show) {
        return;
    }

    connect(&tvShow->episodeListMutable(), SIGNAL(watchCountChanged(int,int)), this, SLOT(updateWatched(int,int)));

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
        QString::number(tvShow->episodeList().numberOfWatchedEpisodes()),
        QString::number(tvShow->episodeList().numberOfEpisodes()),
        QString::number(tvShow->getTotalEpisodes())
    ));
}

void TvShowPage::receivedPlayerSettings(QHttpResponse *resp, const QByteArray& body) {
    if (!tvShow) {
        return;
    }

    std::stringstream ss;
    ss << body.data();
    nw::JsonReader jr(ss);

    if (jr.getErrorMessage().empty()) {
        tvShow->playerSettings.describe(&jr);
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else {
        Server::simpleWrite(resp, 400, "{\"error\": \"no valid number provided in query\"}", mime::json);
    }
    jr.close();
}

void TvShowPage::receivedReleaseGroupPreference(QHttpResponse* resp, const QByteArray& body) {
    if (!tvShow) {
        return;
    }

    std::stringstream ss;
    ss << body.data();
    nw::JsonReader jr(ss);

    if (jr.getErrorMessage().empty()) {
        QStringList groups;
        NwUtils::describeValueArray(jr, "releaseGroupPreference", groups);
        tvShow->setReleaseGroupPreference(groups);
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else {
        Server::simpleWrite(resp, 400, "{\"error\": \"no array provided in query\"}", mime::json);
    }
    jr.close();
}


bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (!tvShow) {
        return false;
    }

    if (req->path() == "/api/page/showDetails") {
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.setState("detailed", true);
        tvShow->write(jw);
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
        return true;
    } else if (req->path().startsWith("/api/page/playerSettings")) {
        // TODO handle this independet from the page

        if (req->method() == QHttpRequest::HTTP_PUT) {
            RequestBodyListener* bodyListener = new RequestBodyListener(resp, this);
            connect(req, SIGNAL(data(QByteArray)), bodyListener, SLOT(onDataReceived(QByteArray)));
            connect(bodyListener, SIGNAL(bodyReceived(QHttpResponse*,const QByteArray&)), this, SLOT(receivedPlayerSettings(QHttpResponse*,const QByteArray&)));
        } else if (req->method() == QHttpRequest::HTTP_GET) {
            std::stringstream ss;
            nw::JsonWriter jw(ss);
            tvShow->playerSettings.describe(&jw);
            jw.close();
            Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
        } else {
            return false;
        }
        return true;
    } else if (req->path().startsWith("/api/page/releaseGroupPreference")) {
        // TODO handle this independet from the page

        if (req->method() == QHttpRequest::HTTP_PUT) {
            RequestBodyListener* bodyListener = new RequestBodyListener(resp, this);
            connect(req, SIGNAL(data(QByteArray)), bodyListener, SLOT(onDataReceived(QByteArray)));
            connect(bodyListener, SIGNAL(bodyReceived(QHttpResponse*,const QByteArray&)), this, SLOT(receivedReleaseGroupPreference(QHttpResponse*,const QByteArray&)));
        } else {
            return false;
        }
        return true;
    }
    return false;
}
