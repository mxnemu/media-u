#ifndef ONLINETVSHOWDATABASE_H
#define ONLINETVSHOWDATABASE_H

#include "nwutils.h"
#include "tvshow.h"
#include "curlresult.h"
#include <QDir>
#include "onlinecredentials.h"

class Library;
namespace OnlineTvShowDatabase {

enum UpdateFilter {
    ufNone = 0,
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
    void updateShow(TvShow& show, const Library& library, const QString identifierKey, UpdateFilter filter = OnlineTvShowDatabase::ufAll) const;

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

    // TODO make this true const again fucking shit
    virtual Entry* bestEntry() = 0;

    const QString searchedQuery;
    QList<Entry*> entries;
};

class Client : public QObject {
    Q_OBJECT
public:
    Client(OnlineCredentials& credentials, QObject* parent = NULL);
    SearchResult* findShow(TvShow& show);

    virtual const QString identifierKey() const = 0;
    virtual UpdateFilter getFilter() { return UpdateFilter::ufNone; }

    const OnlineCredentials&  credentials;

protected:
    virtual SearchResult* search(QString anime) = 0;
};

}

#endif // ONLINETVSHOWDATABASE_H
