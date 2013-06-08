#include "libraryfilter.h"

LibraryFilter::LibraryFilter(QList<TvShow> &shows) : tvShows(shows)
{
}

QList<TvShow *> LibraryFilter::all()
{
    return filter(LibraryFilter::filterAll);
}

QList<TvShow *> LibraryFilter::airing()
{
    return filter(LibraryFilter::filterAiring);
}

QList<TvShow *> LibraryFilter::filter(bool (*filterFunc)(const TvShow &))
{
    QList<TvShow*> filteredList;
    for (int i=0; i < tvShows.length(); ++i) {
        TvShow& show = tvShows[i];
        if (filterFunc(show)) {
            filteredList.append(&show);
        }
    }
    return filteredList;
}

bool LibraryFilter::filterAll(const TvShow &)
{
    return true;
}

bool LibraryFilter::filterAiring(const TvShow & show)
{
    return show.isAiring();
}
