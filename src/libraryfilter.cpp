#include "libraryfilter.h"
#include "server.h"

LibraryFilter::LibraryFilter(QList<TvShow *> &shows, QDir libraryDir) :
    tvShows(shows),
    libraryDir(libraryDir)
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

QList<TvShow *> LibraryFilter::withWallpaper() {
    return filter(LibraryFilter::filterHasWallpaper);
}

QList<TvShow *> LibraryFilter::recentlyWatched() {
    return filter(LibraryFilter::filterRecentlyWatched);
}

QList<TvShow *> LibraryFilter::statusCompleted() {
    TvShow::WatchStatus container = TvShow::completed;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusWatching() {
    TvShow::WatchStatus container = TvShow::watching;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusWaitingForNewEpisodes() {
    TvShow::WatchStatus container = TvShow::waitingForNewEpisodes;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusOnHold() {
    TvShow::WatchStatus container = TvShow::onHold;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusDropped() {
    TvShow::WatchStatus container = TvShow::dropped;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusPlanToWatch() {
    TvShow::WatchStatus container = TvShow::planToWatch;
    return filter(LibraryFilter::filterStatus, &container);
}

bool LibraryFilter::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/library/filter/lists")) {
        sendLists(resp,
                  QList<std::pair<QString, QList<TvShow*> > >() <<
                  std::pair<QString, QList<TvShow*> >("airing", airing()) <<
                  std::pair<QString, QList<TvShow*> >("watching", statusWatching()) <<
                  std::pair<QString, QList<TvShow*> >("waiting-for-new-episodes", statusWaitingForNewEpisodes()) <<
                  std::pair<QString, QList<TvShow*> >("plan-to-watch", statusPlanToWatch()) <<
                  std::pair<QString, QList<TvShow*> >("dropped", statusDropped()) <<
                  std::pair<QString, QList<TvShow*> >("completed", statusCompleted())
        );
    } else if (req->path().startsWith("/api/library/filter/oldLists")) {
        sendLists(resp,
                  QList<std::pair<QString, QList<TvShow*> > >() <<
                  std::pair<QString, QList<TvShow*> >("airing", airing()) <<
                  std::pair<QString, QList<TvShow*> >("all", all()) <<
                  std::pair<QString, QList<TvShow*> >("recently-watched", recentlyWatched())
        );
    } else {
        return false;
    }
    return true;
}

void LibraryFilter::sendLists(QHttpResponse *resp, QList<std::pair<QString, QList<TvShow*> > > lists) {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    jw.push("lists");

    for (int i=0; i < lists.length(); ++i) {
        const std::pair<QString, QList<TvShow*> > & l = lists.at(i);
        jw.describeArray(l.first.toStdString(), "show", l.second.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            const TvShow* show = l.second.at(i);
            show->writeAsListingItem(&jw);
        }
    }

    jw.pop();
    jw.close();
    Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
}

Episode *LibraryFilter::getEpisodeForPath(const QString &path) {
    for (int i=0; i < tvShows.length(); ++i) {
        Episode* episode = tvShows[i]->episodeListMutable().getEpisodeForPath(path);
        if (episode) {
            return episode;
        }
    }
    return NULL;
}

TvShow *LibraryFilter::getShowForRemoteId(int remoteId) {
    foreach (TvShow* show, tvShows) {
        if (show->getRemoteId() == remoteId) {
            return show;
        }
    }
    return NULL;
}

TvShow *LibraryFilter::getRandomShow() {
    return getRandomShow(tvShows);
}

TvShow *LibraryFilter::getRandomShow(const QList<TvShow*>& shows) {
    if (shows.empty()) {
        return NULL;
    }
    int randomIndex = rand() % shows.length();
    if (randomIndex < shows.length()) {
        return shows[randomIndex];
    }
    return NULL;
}

QString LibraryFilter::getRandomWallpaper() {
    TvShow* show = getRandomShow(withWallpaper());
    if (show) {
        return show->randomWallpaper(libraryDir);
    }
    return QString();
}

QList<TvShow *> LibraryFilter::filter(bool (*filterFunc)(const TvShow &, const LibraryFilter&, const void* userData), const void* userData)
{
    QList<TvShow*> filteredList;
    for (int i=0; i < tvShows.length(); ++i) {
        TvShow& show = *tvShows[i];
        if (filterFunc(show, *this, userData)) {
            filteredList.append(&show);
        }
    }
    return filteredList;
}

bool LibraryFilter::filterAll(const TvShow &, const LibraryFilter &, const void *)
{
    return true;
}

bool LibraryFilter::filterAiring(const TvShow & show, const LibraryFilter &, const void* )
{
    return show.isAiring();
}

bool LibraryFilter::filterHasWallpaper(const TvShow &show, const LibraryFilter &filter, const void *)
{
    return !show.wallpapers(filter.getLibraryDir()).isEmpty();
}

bool LibraryFilter::filterRecentlyWatched(const TvShow &show, const LibraryFilter &, const void*) {
    if (show.isCompleted()) {
        return false;
    }
    QDateTime date = show.lastWatchedDate();
    return !date.isNull() && date > QDateTime::currentDateTime().addMonths(-1);
}

bool LibraryFilter::filterStatus(const TvShow &show, const LibraryFilter &, const void *userData) {
    return show.getStatus() == *((const TvShow::WatchStatus*)userData);
}

QDir LibraryFilter::getLibraryDir() const
{
    return libraryDir;
}

