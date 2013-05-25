#include "pagefactory.h"
#include "mainpage.h"

PageFactory::PageFactory(Library &library) : library(library)
{
}

const QString PageFactory::mainPageKey = QString("MainPage");
const QString PageFactory::tvShowsPageKey = QString("TvShowsPage");
const QString PageFactory::tvShowPageKey = QString("TvShowPage");

Page* PageFactory::pageForKey(const QString &key) {
    if (key == PageFactory::mainPageKey) {
        return new MainPage(library);
    } else if (key == PageFactory::tvShowsPageKey) {

    } else if (key == PageFactory::tvShowPageKey) {

    }
    return new MainPage(library);
}
