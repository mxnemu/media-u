#ifndef ONLINETRACKER_H
#define ONLINETRACKER_H

#include <QObject>
#include "tvshow.h"
#include "onlinecredentials.h"

class OnlineTracker : public QObject
{
    Q_OBJECT
public:
    enum UpdateResult {
        invalid,
        failedDueToMissingData,
        failedDueToNetwork,
        success,
        alreadySameAsLocal,
        skipDueToNoChanges
    };

    class Entry {
    public:
        QDateTime lastUpdate;
        int remoteId;
    };

    class EntryList {
    public:
        virtual const Entry* get(const QString trackerIdentifierKey, const TvShow* show) const = 0 ;
        virtual void describe(nw::Describer& de) = 0;
    };

    EntryList* getEntries();

    explicit OnlineTracker(const OnlineCredentials& credentials, QObject *parent = 0);
    virtual UpdateResult updateRemoteImpl(const TvShow* show) = 0;
    virtual bool updateRemote(TvShow* show);
    virtual bool fetchRemote(QList<TvShow*>& shows) = 0;
    virtual const QString identifierKey() const = 0;
signals:

public slots:

protected:
    const OnlineCredentials& credentials;
    EntryList* entries;
};

#endif // ONLINETRACKER_H
