#include "pagefactory.h"
#include "mainpage.h"

PageFactory::PageFactory(Library &library) : library(library)
{
}

QWidget* PageFactory::pageForKey(const QString &key) {
    return new MainPage(library);
}
