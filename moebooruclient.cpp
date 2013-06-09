#include "moebooruclient.h"
#include <QDebug>
#include "filedownloadthread.h"

namespace Moebooru {

Client::Client(QString baseUrl, int limit, Rating ratingFilter) :
    baseUrl(baseUrl),
    ratingFilter(ratingFilter),
    limit(limit)
{
}

CURL *Client::curlClient(QString tag, CurlResult& userdata)
{
    // TODO set limit and search the next page if not enough wallpapers were found that match the ratingfilter
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, QString("%1/post.json?tags=%3").arg(baseUrl, tag).toLocal8Bit().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

FetchThread::FetchThread(Client &client, QList<TvShow *> tvShows, QDir libraryDirectory) :
    client(client),
    tvShows(tvShows),
    libraryDirectory(libraryDirectory)
{
}

void FetchThread::run()
{
    for (int i=0; i < tvShows.length(); ++i) {
        const TvShow* show = tvShows.at(i);
        if (show->numberOfWallpapers(libraryDirectory) < client.getLimit())  {
            SearchResult result = client.fetchPostsBlocking(show->name());
            result.downloadBestResults(show->wallpaperDirectory(libraryDirectory), client.getRatingFilter());
        }
    }
}

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

void SearchResult::downloadBestResults(QDir directory, Rating ratingFilter) {
    int matches = 0;
    QList<FileDownloadThread*> threads;
    for (int i=0; matches < limit && i < entries.length(); ++i) {
        const Entry& entry = entries.at(i);
        Rating rating = entry.ratingFromString();
        if ((ratingFilter & rating) == rating) {
            QString filename = QString("todo-domain_%2").arg(entry.id);
            FileDownloadThread* fileThread = new FileDownloadThread(entry.fileUrl, directory.absoluteFilePath(filename), false);
            //QObject::connect(fileThread, SIGNAL(finished()), fileThread, SLOT(deleteLater()));
            fileThread->start();
            threads.push_back(fileThread);
            matches++;
            qDebug() << "download wallpaper " << entry.fileUrl;
        }

        // blocking to avoid sending to many requests, so we don't get banned
        // lazy! make this a qobject and listen for callback
        while (threads.length() > 1) {
            for (int i=0; i < threads.length(); ++i) {
                if (threads.at(i)->isFinished()) {
                    //delete threads[i]; // they delete themself
                    threads.removeAt(i);
                    --i;
                }
            }
        }
    }
}

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

SearchResult Client::fetchPostsBlocking(QString tagName) {
    if (tagName.isEmpty() || tagName.isNull()) {
        return SearchResult();
    }
    tagName.replace(' ', '_');

    CurlResult userData(this);
    CURL* handle = curlClient(tagName, userData);
    CURLcode error = curl_easy_perform(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for tagquery '" << tagName << "'' with this message:\n";
        userData.print();
    } else {
        SearchResult result(userData.data, limit);
        return result;
    }
    return SearchResult();
}


Rating Client::getRatingFilter() const
{
    return ratingFilter;
}


int Client::getLimit() const
{
    return limit;
}


};


