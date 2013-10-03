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
SearchResult::SearchResult(int limit) :
    limit(limit)
{
}

///////////////////////////////////////////////////////
// Entry
///////////////////////////////////////////////////////
Entry::Entry() {}

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

SearchResult Client::fetchPostsBlocking(QString tagName, int page) {
    if (tagName.isEmpty() || tagName.isNull()) {
        return SearchResult();
    }
    tagName.replace(' ', '_');

    CurlResult userData(this);
    CURL* handle = curlClient(tagName, userData, page);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for tagquery '" << tagName << "'' with this message:\n";
        userData.print();
    } else {
        return parseSearchResult(userData.data, limit);
    }
    return SearchResult();
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


