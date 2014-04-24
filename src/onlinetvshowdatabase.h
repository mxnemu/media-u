#ifndef ONLINETVSHOWDATABASE_H
#define ONLINETVSHOWDATABASE_H

#include "nwutils.h"
#include "tvshow.h"
#include "curlresult.h"
#include <QDir>
#include <QThread>
#include "onlinecredentials.h"

class Library;
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
    void updateShow(TvShow& show, const Library& library, UpdateFilter filter = OnlineTvShowDatabase::ufAll) const;

    virtual int getRemoteId() const = 0;
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
    Client(OnlineCredentials& credentials, QObject* parent = NULL);

    void startUpdate(QList<TvShow *> &showList, const Library& library);
    bool findShow(TvShow& show, const Library& library);

    virtual bool login() = 0;
    virtual SearchResult* search(QString anime) = 0;
    virtual const Entry* bestResult(const SearchResult&) const = 0;

    static const QString identifierKey;

signals:
    void updateFinished();

public slots:
    void threadFinished();
protected:
    OnlineCredentials& credentials;
    Thread* activeThread;
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(Client &client, QList<TvShow*> &shows, const Library& library, QObject *parent);
    void run();

protected:
    Client &client;
    QList<TvShow*> &tvShows;
    const Library& library;
    const int requestSleepPadding;
};


}

#endif // ONLINETVSHOWDATABASE_H
