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
    QString name;
    QString url;
    QDateTime date;
};

class FeedResult {
public:
    virtual ~FeedResult();
    QList<Entry*> entires;
    void removeSameEntries(const FeedResult& other);
    virtual void parse(CurlResult& result) = 0;
};

class Feed {
public:
    Feed(QString url);
    virtual ~Feed();
    virtual void fetch();

protected:
    virtual FeedResult* createFeedResult() = 0;
    CURL* defaultCurlClient(QString url, CurlResult& userdata);

    void setResult(FeedResult* result);

private:
    QString url;
    FeedResult* result;
};

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(TorrentClient& torrentClient, Library& library, QObject *parent = 0);
    virtual ~Client();

    void refetch();

    void addFeed(Feed* feed);
    virtual void addFeed(TvShow* show) = 0;
signals:
    void torrentAvailable(Entry url);

private slots:
    void tvShowChangedStatus(TvShow* show, TvShow::WatchStatus newStatus, TvShow::WatchStatus oldStatus);
protected:
    QList<Feed*> feeds;
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(Client& client, QObject* parent = 0);
    void run();

private:
    Client& client;
    bool toldToStop;
    int refetchInterval;
    int sleeped;
    int sleepStep;
};

} // namespace

#endif // TORRENTRSS_H
