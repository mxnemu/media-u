#ifndef LIBRARYFILTER_H
#define LIBRARYFILTER_H

#include <QList>
#include "tvshow.h"
#include <qhttpconnection.h>
#include <utility>

class LibraryFilter
{
public:
    LibraryFilter(QList<TvShow*>& shows, QDir libraryDir);

    QList<TvShow*> all() const;
    QList<TvShow*> airing() const;
    QList<TvShow*> withWallpaper() const;
    QList<TvShow*> recentlyWatched() const;

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp) const;
    Episode* getEpisodeForPath(const QString& path) const;
    TvShow* getTvShowForPath(const QString& path) const;
    TvShow* getShowForRemoteId(const QString trackerIdentifierKey, int remoteId) const;
    TvShow* getRandomShow() const;
    QString getRandomWallpaper() const;

    QDir getLibraryDir() const;

    QList<TvShow *> statusCompleted() const;
    QList<TvShow *> statusWatching() const;
    QList<TvShow *> statusWaitingForNewEpisodes() const;
    QList<TvShow *> statusOnHold() const;
    QList<TvShow *> statusDropped() const;
    QList<TvShow *> statusPlanToWatch() const;
    void sendLists(QHttpResponse *resp, QList<std::pair<QString, QList<TvShow *> > > lists) const;

    QList<std::pair<QString, QList<TvShow*> > > genLists() const;
    QList<const VideoFile*> missingFiles() const;
    QList<TvShow*> noRemoteId() const;
private:
    typedef bool (*FilterFunction)(const TvShow &, const LibraryFilter&, const void*);

    QList<TvShow*> filter(FilterFunction, const void* userData = NULL) const;

    static bool filterAll(const TvShow&, const LibraryFilter&, const void *userData);
    static bool filterAiring(const TvShow & show, const LibraryFilter&, const void* userData);
    static bool filterHasWallpaper(const TvShow& show, const LibraryFilter& filter, const void* userData);
    static bool filterRecentlyWatched(const TvShow& show, const LibraryFilter&, const void *userData);
    static bool filterStatus(const TvShow& show, const LibraryFilter&, const void* userData);
    static bool filterNoRemoteId(const TvShow& show, const LibraryFilter&, const void*);

    TvShow* getRandomShow(const QList<TvShow *> &shows) const;

    const QList<TvShow*>& tvShows;
    QDir libraryDir;

};

#endif // LIBRARYFILTER_H
