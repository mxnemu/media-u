#include "tvshowpage.h"
#include "ui_tvshowpage.h"

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
}

bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    return false;
}
