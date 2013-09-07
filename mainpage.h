#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "page.h"
#include "library.h"
#include "mainwindow.h"

namespace Ui {
class MainPage;
}

class MainPage : public Page
{
    Q_OBJECT
    
public:
    explicit MainPage(Library& library, MainWindow* mainwindow, QWidget *parent = 0);
    ~MainPage();

    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    
private slots:
    void on_settingsButton_clicked();

private:
    Ui::MainPage *ui;
    Library& library;
    QList<TvShow*> airingShows;
    QList<TvShow*> allShows;
    MainWindow* mainwindow;
};

#endif // MAINPAGE_H
