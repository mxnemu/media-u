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
        EntryList();
        virtual const Entry* get(const QString trackerIdentifierKey, const TvShow* show) const = 0 ;
        virtual void describe(nw::Describer& de) = 0;
        bool tooOld() const;
        const QDateTime fetchTime;
    };

    explicit OnlineTracker(const OnlineCredentials& credentials, QObject *parent = 0);
    virtual UpdateResult updateRemoteImpl(const TvShow* show, const EntryList& entries) const = 0;
    virtual bool updateRemote(TvShow* show);
    virtual EntryList* fetchRemote() const = 0; ///< Must create a new EntryList using online Data, or NULL on error
    virtual const QString identifierKey() const = 0;

    const OnlineCredentials& credentials;

private:
    /// Either returns cached entries, or fetches new ones when cached are too old
    EntryList* satisfyingEntries();

    EntryList* cachedEntries;
};

#endif // ONLINETRACKER_H
