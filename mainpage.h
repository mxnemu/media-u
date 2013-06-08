#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "page.h"
#include "library.h"

namespace Ui {
class MainPage;
}

class MainPage : public Page
{
    Q_OBJECT
    
public:
    explicit MainPage(Library& library, QWidget *parent = 0);
    ~MainPage();

    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    
private:
    Ui::MainPage *ui;
    Library& library;
    QList<TvShow*> airingShows;
    QList<TvShow*> allShows;
};

#endif // MAINPAGE_H
