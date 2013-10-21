#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <QList>
#include "tvshow.h"
#include <qhttpconnection.h>

class LibraryFilter
{
public:
    LibraryFilter(QList<TvShow*>& shows, QDir libraryDir);

    QList<TvShow*> all();
    QList<TvShow*> airing();
    QList<TvShow*> withWallpaper();
    QList<TvShow*> recentlyWatched();

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);
    MovieFile* getEpisodeForPath(const QString& path);
    TvShow* getShowForRemoteId(int remoteId);
    TvShow* getRandomShow();
    QString getRandomWallpaper();

    QDir getLibraryDir() const;

private:
    QList<TvShow*> filter(bool (*filterFunc)(const TvShow &, const LibraryFilter&));

    static bool filterAll(const TvShow&, const LibraryFilter&);
    static bool filterAiring(const TvShow & show, const LibraryFilter&);
    static bool filterHasWallpaper(const TvShow& show, const LibraryFilter& filter);
    static bool filterRecentlyWatched(const TvShow& show, const LibraryFilter&);

    TvShow* getRandomShow(const QList<TvShow *> &shows);

    QList<TvShow*>& tvShows;
    QDir libraryDir;
};

#endif // LIBRARYFILTER_H
