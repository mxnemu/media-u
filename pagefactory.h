#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <QWidget>
#include <QString>
#include "library.h"
#include "page.h"

class PageFactory
{
public:
    PageFactory(Library& library);
    Page* pageForKey(const QString& key);


    static const QString mainPageKey;
    static const QString tvShowsPageKey;
    static const QString tvShowPageKey;

private:
    Library& library;
};

#endif // PAGEFACTORY_H
