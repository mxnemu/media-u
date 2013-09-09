#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <QList>
#include "tvshow.h"
#include <qhttpconnection.h>

class LibraryFilter
{
public:
    LibraryFilter(QList<TvShow>& shows);

    QList<TvShow*> all();
    QList<TvShow*> airing();

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);
    MovieFile* getEpisodeForPath(const QString& path);

private:
    QList<TvShow*> filter(bool (*filterFunc)(const TvShow &));

    static bool filterAll(const TvShow&);
    static bool filterAiring(const TvShow & show);

    QList<TvShow>& tvShows;
};

#endif // LIBRARYFILTER_H
