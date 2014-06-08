#include "libraryfilter.h"
#include "server.h"

LibraryFilter::LibraryFilter(QList<TvShow *> &shows, QDir libraryDir) :
    tvShows(shows),
    libraryDir(libraryDir)
{
}

QList<TvShow *> LibraryFilter::all() const
{
    return filter(LibraryFilter::filterAll);
}

QList<TvShow *> LibraryFilter::airing() const
{
    return filter(LibraryFilter::filterAiring);
}

QList<TvShow *> LibraryFilter::withWallpaper() const {
    return filter(LibraryFilter::filterHasWallpaper);
}

QList<TvShow *> LibraryFilter::recentlyWatched() const{
    return filter(LibraryFilter::filterRecentlyWatched);
}

QList<TvShow *> LibraryFilter::statusCompleted() const{
    TvShow::WatchStatus container = TvShow::completed;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusWatching() const{
    TvShow::WatchStatus container = TvShow::watching;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusWaitingForNewEpisodes() const{
    TvShow::WatchStatus container = TvShow::waitingForNewEpisodes;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusOnHold() const{
    TvShow::WatchStatus container = TvShow::onHold;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusDropped() const{
    TvShow::WatchStatus container = TvShow::dropped;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<TvShow *> LibraryFilter::statusPlanToWatch() const{
    TvShow::WatchStatus container = TvShow::planToWatch;
    return filter(LibraryFilter::filterStatus, &container);
}

QList<const VideoFile *> LibraryFilter::missingFiles() const {
    QList<const VideoFile *> missing;
    foreach (TvShow* show, tvShows) {
        missing << show->episodeList().missingFiles();
    }
    return missing;
}

QList<TvShow *> LibraryFilter::noRemoteId() const {
    return filter(LibraryFilter::filterNoRemoteId);
}

bool LibraryFilter::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) const {
    if (req->path().startsWith("/api/library/filter/lists")) {
        sendLists(resp,genLists());

    } else if (req->path().startsWith("/api/library/filter/slowLists")) {
        sendLists(resp,
                  QList<std::pair<QString, QList<TvShow*> > >() <<
                  std::make_pair("airing", airing()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::watching), statusWatching()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::waitingForNewEpisodes), statusWaitingForNewEpisodes()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::planToWatch), statusPlanToWatch()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::onHold), statusOnHold()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::dropped), statusDropped()) <<
                  std::make_pair(TvShow::watchStatusToString(TvShow::completed), statusCompleted())
        );
    } else if (req->path().startsWith("/api/library/filter/oldLists")) {
        sendLists(resp,
                  QList<std::pair<QString, QList<TvShow*> > >() <<
                  std::make_pair("airing", airing()) <<
                  std::make_pair("all", all()) <<
                  std::make_pair("recently-watched", recentlyWatched())
        );
    } else if (req->path().startsWith("/api/library/filter/missingFiles")) {
        QList<const VideoFile*> missing = this->missingFiles();
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        foreach (const VideoFile* mf, missing) {
            mf->writeForApi(jw);
        }
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
    } else if (req->path().startsWith("/api/library/filter/noRemoteId")) {
        sendLists(resp,
                  QList<std::pair<QString, QList<TvShow*> > >() <<
                  std::make_pair("noRemoteId", noRemoteId())
        );
    } else {
        return false;
    }
    return true;
}

QList<std::pair<QString, QList<TvShow*> > > LibraryFilter::genLists() const {
    QList<TvShow*> airing;
    QList<TvShow*> watching;
    QList<TvShow*> waitingForNewEpisodes;
    QList<TvShow*> planToWatch;
    QList<TvShow*> onHold;
    QList<TvShow*> dropped;
    QList<TvShow*> completed;

    for (int i=0; i < tvShows.length(); ++i) {
        TvShow* show = tvShows[i];
        TvShow::WatchStatus status = show->getStatus();

        if (show->isAiring() && status != TvShow::dropped) airing.push_back(show);
        switch (status) {
        case TvShow::watching: watching.push_back(show); break;
        case TvShow::waitingForNewEpisodes: waitingForNewEpisodes.push_back(show); break;
        case TvShow::planToWatch: planToWatch.push_back(show); break;
        case TvShow::onHold: onHold.push_back(show); break;
        case TvShow::dropped: dropped.push_back(show); break;
        case TvShow::completed: completed.push_back(show); break;
        default: break;
        }
    }
    return QList<std::pair<QString, QList<TvShow*> > >() <<
            std::make_pair(TvShow::watchStatusToString(TvShow::watching), watching) <<
            std::make_pair(TvShow::watchStatusToString(TvShow::waitingForNewEpisodes), waitingForNewEpisodes) <<
            std::make_pair("airing", airing) <<
            std::make_pair(TvShow::watchStatusToString(TvShow::planToWatch), planToWatch) <<
            std::make_pair(TvShow::watchStatusToString(TvShow::onHold), onHold) <<
            std::make_pair(TvShow::watchStatusToString(TvShow::dropped), dropped) <<
            std::make_pair(TvShow::watchStatusToString(TvShow::completed), completed);
}

void LibraryFilter::sendLists(QHttpResponse *resp, QList<std::pair<QString, QList<TvShow*> > > lists) const {
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

Episode *LibraryFilter::getEpisodeForPath(const QString &path) const {
    for (int i=0; i < tvShows.length(); ++i) {
        Episode* episode = tvShows[i]->episodeList().getEpisodeForPath(path);
        if (episode) {
            return episode;
        }
    }
    return NULL;
}

TvShow* LibraryFilter::getTvShowForPath(const QString &path) const {
    for (int i=0; i < tvShows.length(); ++i) {
        Episode* episode = tvShows[i]->episodeList().getEpisodeForPath(path);
        if (episode) {
            return tvShows[i];
        }
    }
    return NULL;
}

TvShow *LibraryFilter::getShowForRemoteId(const QString trackerIdentifierKey, int remoteId) const {
    foreach (TvShow* show, tvShows) {
        if (show->matchesRemote(trackerIdentifierKey, remoteId)) {
            return show;
        }
    }
    return NULL;
}

TvShow *LibraryFilter::getRandomShow() const {
    return getRandomShow(tvShows);
}

TvShow *LibraryFilter::getRandomShow(const QList<TvShow*>& shows) const {
    if (shows.empty()) {
        return NULL;
    }
    int randomIndex = rand() % shows.length();
    if (randomIndex < shows.length()) {
        return shows[randomIndex];
    }
    return NULL;
}

QString LibraryFilter::getRandomWallpaper() const {
    TvShow* show = getRandomShow(withWallpaper());
    if (show) {
        return show->randomWallpaper(libraryDir);
    }
    return QString();
}

QList<TvShow *> LibraryFilter::filter(FilterFunction filterFunc, const void* userData) const
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

bool LibraryFilter::filterNoRemoteId(const TvShow &show, const LibraryFilter &, const void *) {
    return show.hasNoRemoteIds();
}

QDir LibraryFilter::getLibraryDir() const
{
    return libraryDir;
}

