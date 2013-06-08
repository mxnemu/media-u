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

    }
}

bool TvShowPage::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    return false;
}
