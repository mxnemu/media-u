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

bool LibraryFilter::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/library/filter/lists")) {
        QList<TvShow *> airingShows = airing();
        QList<TvShow *> allShows = all();
        QList<TvShow *> recent = recentlyWatched();
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.push("lists");
        jw.describeArray("airing", "show", airingShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = airingShows.at(i);
            std::string name = show->name().toStdString();
            jw.describe("name", name);
        }

        jw.describeArray("recently-watched", "show", recent.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = recent.at(i);
            std::string name = show->name().toStdString();
            jw.describe("name", name);
        }

        jw.describeArray("all", "show", allShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = allShows.at(i);
            std::string name = show->name().toStdString();
            jw.describe("name", name);
        }

        jw.pop();
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
    } else {
        return false;
    }
    return true;
}

MovieFile *LibraryFilter::getEpisodeForPath(const QString &path) {
    for (int i=0; i < tvShows.length(); ++i) {
        MovieFile* episode = tvShows[i]->getEpisodeForPath(path);
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

QList<TvShow *> LibraryFilter::filter(bool (*filterFunc)(const TvShow &, const LibraryFilter&))
{
    QList<TvShow*> filteredList;
    for (int i=0; i < tvShows.length(); ++i) {
        TvShow& show = *tvShows[i];
        if (filterFunc(show, *this)) {
            filteredList.append(&show);
        }
    }
    return filteredList;
}

bool LibraryFilter::filterAll(const TvShow &, const LibraryFilter &)
{
    return true;
}

bool LibraryFilter::filterAiring(const TvShow & show, const LibraryFilter &)
{
    return show.isAiring();
}

bool LibraryFilter::filterHasWallpaper(const TvShow &show, const LibraryFilter &filter)
{
    return !show.wallpapers(filter.getLibraryDir()).isEmpty();
}

bool LibraryFilter::filterRecentlyWatched(const TvShow &show, const LibraryFilter &) {
    QDateTime date = show.lastWatchedDate();
    return !date.isNull() && date > QDateTime::currentDateTime().addMonths(-1);
}

QDir LibraryFilter::getLibraryDir() const
{
    return libraryDir;
}

