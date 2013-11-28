#ifndef TORRENTRSS_H
#define TORRENTRSS_H

#include <QObject>
#include <QThread>

namespace TorrentRss {

class Entry {
public:
    QString name;
    QString url;
};

class FeedResult {
public:
    QList<Entry> entires;
    void removeSameEntries(const FeedResult& other);
};

class Feed {
public:
    QString url;
    FeedResult fetch();
};

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QString url, QObject *parent = 0);

signals:
    void torrentAvailable(Entry url);
public slots:

protected:
    QList<Feed> feeds;
};

class Thread : public QThread {
    Q_OBJECT
public:
    void run();
};

} // namespace

#endif // TORRENTRSS_H
