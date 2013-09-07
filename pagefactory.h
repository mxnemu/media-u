#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <QWidget>
#include <QString>
#include "library.h"
#include "page.h"

// TODO why did I write this bullshit bloatclass? it's completly useless
class MainWindow;
class PageFactory
{
public:
    PageFactory(Library& library, MainWindow* mainwindow);
    Page* pageForKey(const QString& key);


    static const QString mainPageKey;
    static const QString tvShowsPageKey;
    static const QString tvShowPageKey;
    static const QString settingsPageKey;

private:
    Library& library;
    MainWindow* mainwindow;
};

#endif // PAGEFACTORY_H
