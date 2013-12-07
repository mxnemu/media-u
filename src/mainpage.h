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
    void onShowAdded(TvShow* show);
    void setRandomWallpaperAfterSearch();
    void setRandomWallpaper(QString path = QString());

private:
    Ui::MainPage *ui;
    Library& library;
    MainWindow* mainwindow;
};

#endif // MAINPAGE_H
