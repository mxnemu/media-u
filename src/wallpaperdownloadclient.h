#ifndef WALLPAPERDOWNLOADCLIENT_H
#define WALLPAPERDOWNLOADCLIENT_H

#include <curl/curl.h>
#include <QString>
#include <QThread>
#include <QDir>
#include <sstream>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "filedownloadthread.h"
#include "wallpaperdownloadclient.h"

/// base to dl images form common wallpaper hosting sites
namespace WallpaperDownload {

enum Rating {
    ratingInvalid = 0,
    ratingSafe = 1 << 0,
    ratingQuestionable = 1 << 1,
    ratingExplicit = 1 << 2
};

class Entry {
public:
    Entry(nw::JsonReader& jr);
    QString id;
    QStringList tags;
    QString fileUrl;
    QString sampleUrl;
    QString previewUrl;
    QString rating;

    Rating ratingFromString() const;
};

class SearchResult {
public:
    SearchResult(std::stringstream& ss, int limit);
    SearchResult();

    QList<Entry> entries;
private:
    int limit;
};

class Client
{
public:
    Client(QString baseUrl, int limit = 10, Rating ratingFilter = ratingSafe);

    virtual SearchResult fetchPostsBlocking(QString tagName, int page = 1) = 0;

    Rating getRatingFilter() const;
    int getLimit() const;

    void downloadBestResults(QDir directory, const QList<Entry> &entries);

protected:
    virtual CURL* curlClient(QString tag, CurlResult& userdata, const unsigned int page = 1) = 0;

    QString baseUrl;
    Rating ratingFilter;
    int limit;
};

class FetchThread : public QThread {
public:
    FetchThread(Client& client, QList<TvShow*> tvShows, QDir libraryDirectory, QObject* parent);
    void run();
private:
    Client& client;
    QList<TvShow*> tvShows;
    QDir libraryDirectory;
};

}

#endif // WALLPAPERDOWNLOADCLIENT_H
