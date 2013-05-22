#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <QWidget>
#include <QString>
#include "library.h"

class PageFactory
{
public:
    PageFactory(Library& library);
    QWidget* pageForKey(const QString& key);

private:
    Library& library;
};

#endif // PAGEFACTORY_H
