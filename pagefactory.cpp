#include "pagefactory.h"
#include "mainpage.h"
#include "tvshowpage.h"
#include "settingspage.h"

PageFactory::PageFactory(Library &library, MainWindow* mainwindow) : library(library), mainwindow(mainwindow)
{
}

const QString PageFactory::mainPageKey = QString("MainPage");
const QString PageFactory::tvShowsPageKey = QString("TvShowsPage");
const QString PageFactory::tvShowPageKey = QString("TvShowPage");
const QString PageFactory::settingsPageKey = QString("SettingsPage");

Page* PageFactory::pageForKey(const QString &key) {
    if (key == PageFactory::mainPageKey) {
        return new MainPage(library, mainwindow);
    } else if (key == PageFactory::tvShowsPageKey) {

    } else if (key == PageFactory::tvShowPageKey) {
        return new TvShowPage(library);
    } else if (key == PageFactory::settingsPageKey) {
        return new SettingsPage(library);
    }
    return new MainPage(library, mainwindow);
}
