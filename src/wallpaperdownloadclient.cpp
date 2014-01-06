#include "wallpaperdownloadclient.h"

#include "moebooruclient.h"
#include <QDebug>
#include <QUrl>
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
        SearchResult allResults(client.getLimit());
        for (int page=1; show->numberOfWallpapers(libraryDirectory) < client.getLimit() && !noEntriesLeft; ++page)  {
            SearchResult result = client.fetchPostsBlocking(show, page);
            result.sortEntries();
            client.downloadResults(show->wallpaperDirectory(libraryDirectory), result.entries, true);
            noEntriesLeft = result.entries.empty();
            allResults.entries << result.entries;
        }
        if (allResults.entries.length() == 0) {
            emit noEntriesFound(show);
            continue;
        }
        if (show->numberOfWallpapers(libraryDirectory) < client.getLimit()) {
            allResults.sortEntries();
            client.downloadResults(show->wallpaperDirectory(libraryDirectory), allResults.entries, false);
        }
    }
}

void FetchThread::append(QList<TvShow*> tvShows) {
    foreach (TvShow* t, tvShows) {
        if (!this->tvShows.contains(t)) {
            this->tvShows.append(t);
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

void SearchResult::sortEntries() {
    qSort(entries);
}

///////////////////////////////////////////////////////
// Entry
///////////////////////////////////////////////////////
Entry::Entry() {}

// TODO move into another function so the method name actually makes sense
// reverse results to get desc order (this is stupid)
bool Entry::operator <(const Entry &b) const {
    // don't dl gifs!
    if (QFileInfo(QUrl(fileUrl).path()).suffix() == "gif") {
        return false;
    }
    bool thisRatio = hasGoodAspectRatio();
    bool bRatio = b.hasGoodAspectRatio();
    if (thisRatio && bRatio) {
        if (score == b.score) {
            if (width == b.width) {
                return height > b.height;
            }
            return width > b.width;
        }
        return score > b.score;
    }
    return thisRatio && !bRatio;
}

bool Entry::hasGoodAspectRatio() const {
    const float aspectRatio = (float)width/(float)height;
    const float minRatio = 1.2;
    const float maxRatio = 2.35; // cinema format
    return aspectRatio >= minRatio && aspectRatio <= maxRatio;
}

bool Entry::isGoodWallpaper() const {
    if (QFileInfo(QUrl(fileUrl).path()).suffix() == "gif") {
        return false;
    }
    return hasGoodAspectRatio();
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
    limit(limit),
    hostname(QUrl(baseUrl).host())
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

SearchResult Client::fetchPostsBlocking(const TvShow* show, int page) {
    QString tagName = show->name();
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

void Client::downloadResults(QDir directory, const QList<Entry>& entries, bool onlyTheBest) {
    QList<FileDownloadThread*> threads;
    for (int i=0; matches < limit && i < entries.length(); ++i) {
        const Entry& entry = entries.at(i);
        Rating rating = entry.ratingFromString();
        if ((ratingFilter & rating) == rating) {
            if (!onlyTheBest || entry.isGoodWallpaper()) {
                QString filename = QString("%1_%2").arg(hostname, entry.id);
                FileDownloadThread* fileThread = new FileDownloadThread(entry.fileUrl, directory.absoluteFilePath(filename), false);
                connect(fileThread, SIGNAL(finished()), fileThread, SLOT(deleteLater()));
                connect(fileThread, SIGNAL(downloadSucceeded(QString)), this, SLOT(onWallpaperDownloadSucceeded(QString)));
                fileThread->start();
                threads.push_back(fileThread);
            }
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

void Client::onWallpaperDownloadSucceeded(QString path)  {
    emit wallpaperDownloaded(path);
    matches++;
}


}


