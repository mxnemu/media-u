#ifndef ONLINETVSHOWDATABASE_H
#define ONLINETVSHOWDATABASE_H

#include "nwutils.h"
#include "tvshow.h"
#include "curlresult.h"
#include <QDir>
#include <QThread>


namespace OnlineTvShowDatabase {

enum UpdateFilter {
    ufInvalid = 0,
    ufSynopsis = 1 << 1,
    ufTitle = 1 << 2,
    ufRelations = 1 << 3,
    ufAiringDates = 1 << 4,
    ufSynonyms = 1 << 5,
    ufRemoteId = 1 << 6,
    ufImage = 1 << 7,
    ufAll = ((unsigned int)-1)
};


class Entry {
public:
    Entry();
    virtual ~Entry();
    void updateShow(TvShow& show, QDir &libraryDir, UpdateFilter filter = OnlineTvShowDatabase::ufAll) const;

    virtual void updateSynopsis(TvShow& show) const = 0;
    virtual void updateTitle(TvShow& show) const = 0;
    virtual void updateRemoteId(TvShow& show) const = 0;
    virtual void updateRelations(TvShow& show) const = 0;
    virtual void updateAiringDates(TvShow& show) const = 0;
    virtual void updateSynonyms(TvShow& show) const = 0;
    virtual void updateImage(TvShow& show, QDir libraryDir) const = 0;
};

class SearchResult {
public:
    SearchResult(QString searchedQuery = QString());
    virtual ~SearchResult();

    void describe(nw::Describer* const de);
    QString searchedQuery;
    QList<Entry*> entries;
};


class Thread;
class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject* parent = NULL);

    void startUpdate(QList<TvShow *> &showList, QDir libraryDir);
    bool updateShow(TvShow& show, QDir &libraryDir);
    virtual SearchResult* search(QString anime) = 0;
    virtual const Entry* bestResult(const SearchResult&) const = 0;

signals:
    void updateFinished();

public slots:
    void threadFinished();
private:
    Thread* activeThread;
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(Client &client, QList<TvShow*> &shows, QDir libraryDir, QObject *parent);

    void run();

protected:
    Client &client;
    QList<TvShow*> &tvShows;
    QDir libraryDir;
};


}

#endif // ONLINETVSHOWDATABASE_H
