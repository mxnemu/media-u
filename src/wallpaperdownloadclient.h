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
    Entry();
    QString id;
    QStringList tags;
    QString fileUrl;
    QString sampleUrl;
    QString previewUrl;
    QString rating;
    int width;
    int height;
    int score;

    bool operator <(const Entry& b) const;
    bool hasGoodAspectRatio() const;
    bool isGoodWallpaper() const;

    Rating ratingFromString() const;
};

class SearchResult {
public:
    SearchResult(int limit = 10);
    void sortEntries();

    QList<Entry> entries;
private:
    int limit;
};

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QString baseUrl, int limit = 10, Rating ratingFilter = ratingSafe);

    SearchResult fetchPostsBlocking(QString tagName, int page = 1);

    Rating getRatingFilter() const;
    int getLimit() const;

    void downloadResults(QDir directory, const QList<Entry> &entries, bool onlyTheBest);

signals:
    void wallpaperDownloaded(QString path);

private slots:
    void onWallpaperDownloadSucceeded(QString path);

protected:
    virtual Entry parseEntry(nw::Describer *de) = 0;
    virtual SearchResult parseSearchResult(std::stringstream &, int limit) = 0;
    virtual CURL* curlClient(QString tag, CurlResult& userdata, const unsigned int page = 1) = 0;

    QString baseUrl;
    Rating ratingFilter;
    int limit;
    int matches;
private:
    const QString hostname;
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
