#ifndef TVSHOWPAGE_H
#define TVSHOWPAGE_H

#include <QWidget>
#include "page.h"
#include "library.h"

namespace Ui {
class TvShowPage;
}

class TvShowPage : public Page
{
    Q_OBJECT
    
public:
    explicit TvShowPage(Library& library, QWidget *parent = 0);
    ~TvShowPage();

    void initFromQuery(const QString &initString);

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);
    
    void setTvShow(TvShow *show);
private:
    Ui::TvShowPage *ui;
    Library& library;
    TvShow* tvShow;
};

#endif // TVSHOWPAGE_H
