#ifndef TORRENTRSS_H
#define TORRENTRSS_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "curlresult.h"
#include "library.h"
#include "torrentclient.h"

namespace TorrentRss {

class Entry {
public:
    virtual ~Entry() {};
    QString name;
    QString url;
    QDateTime date;

    virtual bool isCandidateForAutoDownload(QString,int,QString) { return false;};
};

class FeedResult {
public:
    virtual ~FeedResult();
    QList<Entry*> entires;
    void removeSameEntries(const FeedResult& other);
    virtual void parse(CurlResult& result) = 0;
};

class Feed : public QObject {
    Q_OBJECT
public:
    Feed(QString url, TvShow* tvShow = NULL);
    virtual ~Feed();
    virtual void fetch();
    TvShow* getTvShow();

    Entry* candidateForAutoDownload();

signals:
    void foundCandidateForAutoDownload(Entry entry);
protected:
    virtual FeedResult* createFeedResult() = 0;
    CURL* defaultCurlClient(QString url, CurlResult& userdata);

    void setResult(FeedResult* result);

private:
    FeedResult* result;
    QString url;
    TvShow* tvShow;
};

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(TorrentClient& torrentClient, Library& library, QObject *parent = 0);
    virtual ~Client();

    void refetch();

    void addFeed(Feed* feed);
    virtual void addFeed(TvShow* show) = 0;
    void removeFeed(Feed* feed);
    void connectLibrary();
signals:
    void torrentAvailable(Entry url);

public slots:
    void addFeedsForWaitingShows();
    void autoDownloadEntry(Entry entry);

private slots:
    void tvShowChangedStatus(TvShow* show, TvShow::WatchStatus newStatus, TvShow::WatchStatus oldStatus);
protected:
    QList<Feed*> feeds;
private:
    Library& library;
    TorrentClient& torrentClient;
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(Client& client, QObject* parent = 0);
    ~Thread();
    void run();

    bool toldToStop;
private:
    Client& client;
    int refetchInterval;
    int sleeped;
    int sleepStep;
};

} // namespace

#endif // TORRENTRSS_H
