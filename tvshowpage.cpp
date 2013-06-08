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
        ui->title->setText(show->name());
        qDebug() << show->coverPath(library.getDirectory());
        if (show->coverPath(library.getDirectory()).length() > 0) {
            QPixmap pix(show->coverPath(library.getDirectory()));
            ui->cover->setPixmap(pix);
        }
    }
}

bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    return false;
}
