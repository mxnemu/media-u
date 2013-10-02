#include "wallpaperdownloadclient.h"

#include "moebooruclient.h"
#include <QDebug>
#include "filedownloadthread.h"

namespace WallpaperDownload {

///////////////////////////////////////////////////////
// FetchThread
///////////////////////////////////////////////////////
FetchThread::FetchThread(Client &client, QList<TvShow *> tvShows, QDir libraryDirectory, QObject *parent) :
    QThread(parent),
    client(client),
    tvShows(tvShows),
    libraryDirectory(libraryDirectory)
{
}

void FetchThread::run()
{
    for (int i=0; i < tvShows.length(); ++i) {
        const TvShow* show = tvShows.at(i);
        bool noEntriesLeft = false;
        for (int page=1; show->numberOfWallpapers(libraryDirectory) < client.getLimit() && !noEntriesLeft; ++page)  {
            SearchResult result = client.fetchPostsBlocking(show->name(), page);
            client.downloadBestResults(show->wallpaperDirectory(libraryDirectory), result.entries);
            noEntriesLeft = result.entries.empty();
        }
    }
}

///////////////////////////////////////////////////////
// SearchResult
///////////////////////////////////////////////////////
SearchResult::SearchResult() {}

SearchResult::SearchResult(std::stringstream &ss, int limit) :
    limit(limit)
{
    nw::JsonReader jr(ss);
    jr.describeArray("","", 0);
    for (int i=0; jr.enterNextElement(i); ++i) {
        entries.push_back(Entry(jr));
    }
}

///////////////////////////////////////////////////////
// Entry
///////////////////////////////////////////////////////
Entry::Entry(nw::JsonReader &jr)
{
    NwUtils::describe(jr, "id", id);
    NwUtils::describe(jr, "rating", rating);
    NwUtils::describe(jr, "file_url", fileUrl);
    NwUtils::describe(jr, "sample_url", sampleUrl);
    NwUtils::describe(jr, "preview_url", previewUrl);
    NwUtils::describe(jr, "tags", tags);
}

Rating Entry::ratingFromString() const {
    if (rating == "s") return ratingSafe;
    if (rating == "q") return ratingQuestionable;
    if (rating == "e") return ratingExplicit;
    return ratingInvalid;
}

///////////////////////////////////////////////////////
// CLIENT
///////////////////////////////////////////////////////
Client::Client(QString baseUrl, int limit, Rating ratingFilter) :
    baseUrl(baseUrl),
    ratingFilter(ratingFilter),
    limit(limit)
{
}

Rating Client::getRatingFilter() const
{
    return ratingFilter;
}


int Client::getLimit() const
{
    return limit;
}

void Client::downloadBestResults(QDir directory, const QList<Entry>& entries) {
    int matches = 0;
    QList<FileDownloadThread*> threads;
    for (int i=0; matches < limit && i < entries.length(); ++i) {
        const Entry& entry = entries.at(i);
        Rating rating = entry.ratingFromString();
        if ((ratingFilter & rating) == rating) {
            QString filename = QString("todo-domain_%1").arg(entry.id);
            FileDownloadThread* fileThread = new FileDownloadThread(entry.fileUrl, directory.absoluteFilePath(filename), false);
            //QObject::connect(fileThread, SIGNAL(finished()), fileThread, SLOT(deleteLater()));
            fileThread->start();
            threads.push_back(fileThread);
            matches++;
        }

        // blocking to avoid sending to many requests, so we don't get banned
        // lazy! make this a qobject and listen for callback
        while (threads.length() > 1) {
            for (int i=0; i < threads.length(); ++i) {
                if (threads.at(i)->isFinished()) {
                    delete threads[i];
                    threads.removeAt(i);
                    --i;
                }
            }
        }
    }
}


};


