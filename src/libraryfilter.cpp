#include "libraryfilter.h"
#include "server.h"

LibraryFilter::LibraryFilter(QList<TvShow *> &shows) : tvShows(shows)
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

bool LibraryFilter::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/library/filter/lists")) {
        QList<TvShow *> airingShows = airing();
        QList<TvShow *> allShows = all();
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.push("lists");
        jw.describeArray("airing", "show", airingShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = airingShows.at(i);
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
        Server::simpleWrite(resp, 200, ss.str().data());
        qDebug() << "resp on lists" << ss.str().data();
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

QList<TvShow *> LibraryFilter::filter(bool (*filterFunc)(const TvShow &))
{
    QList<TvShow*> filteredList;
    for (int i=0; i < tvShows.length(); ++i) {
        TvShow& show = *tvShows[i];
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
