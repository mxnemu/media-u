#ifndef MOEBOORUCLIENT_H
#define MOEBOORUCLIENT_H

#include <curl/curl.h>
#include <QString>
#include <QThread>
#include <QDir>
#include <sstream>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "filedownloadthread.h"

namespace Moebooru {

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
    void downloadBestResults(QDir directory, Rating ratingFilter);

private:
    QList<Entry> entries;
    int limit;
};

class Client
{
public:
    Client(QString baseUrl, int limit = 10, Rating ratingFilter = ratingSafe);

    SearchResult fetchPostsBlocking(QString tagName);

    Rating getRatingFilter() const;
    int getLimit() const;

private:
    CURL* curlClient(QString tag, CurlResult& userdata);

    QString baseUrl;
    Rating ratingFilter;
    int limit;
};

class FetchThread : public QThread {
public:
    FetchThread(Client& client, QList<TvShow*> tvShows, QDir libraryDirectory);
    void run();
private:
    Client& client;
    QList<TvShow*> tvShows;
    QDir libraryDirectory;
};

}

#endif // MOEBOORUCLIENT_H
