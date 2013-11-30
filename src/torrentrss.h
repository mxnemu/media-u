#ifndef TORRENTRSS_H
#define TORRENTRSS_H

#include <QObject>
#include <QThread>
#include "curlresult.h"
#include <QDateTime>

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
    Feed();
    virtual ~Feed();
    QString url;
    virtual void fetch();

protected:
    virtual FeedResult* createFeedResult() = 0;
    CURL* defaultCurlClient(QString url, CurlResult& userdata);

    void setResult(FeedResult* result);

private:
    FeedResult* result;
};

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    virtual ~Client();

    void refetch();

    void addFeed(Feed* feed);
signals:
    void torrentAvailable(Entry url);
public slots:

protected:
    QList<Feed*> feeds;
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(QObject* parent = 0);
    void run();

private:
    Client client;
    bool toldToStop;
    int refetchInterval;
    int sleeped;
    int sleepStep;
};

} // namespace

#endif // TORRENTRSS_H
