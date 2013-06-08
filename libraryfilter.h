#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <QList>
#include "tvshow.h"

class LibraryFilter
{
public:
    LibraryFilter(QList<TvShow>& shows);

    QList<TvShow*> all();
    QList<TvShow*> airing();

private:
    QList<TvShow*> filter(bool (*filterFunc)(const TvShow &));

    static bool filterAll(const TvShow&);
    static bool filterAiring(const TvShow & show);

    QList<TvShow>& tvShows;
};

#endif // LIBRARYFILTER_H
