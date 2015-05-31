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
        virtual ~Entry();
        QDateTime lastUpdate;
        int remoteId;
        int watchedEpisodes;

        bool localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const;
        bool remoteIsUpToDate(const TvShow* show) const;
        bool syncConflict(const QString trackerIdentifier, const TvShow* show) const;

        virtual bool remoteIsEq(const TvShow* show) const = 0;
    };

    class EntryList {
    public:
        EntryList();
        virtual ~EntryList();
        virtual const Entry* get(const QString trackerIdentifierKey, const TvShow* show) const = 0;
        virtual void makeSureLocalIsUpdated(const QString trackerIdentifierKey, TvShow* show) const = 0;
        virtual void describe(nw::Describer& de) = 0;

        bool tooOld() const;
        const QDateTime fetchTime;
    };

    explicit OnlineTracker(const OnlineCredentials& credentials, QObject *parent = 0);
    virtual ~OnlineTracker();
    bool updateRemote(TvShow* show);
    virtual EntryList* fetchRemote() = 0; ///< Must create a new EntryList using online Data, or NULL on error
    virtual const QString identifierKey() const = 0;

    const OnlineCredentials& credentials;

protected:
    virtual UpdateResult updateinOnlineTrackerOrAdd(const TvShow* show, const QString& type) const = 0;

private:
    /// Either returns cached entries, or fetches new ones when cached are too old
    EntryList* satisfyingEntries();
    UpdateResult updateRemoteImpl(const TvShow* show, const EntryList& entries) const;

    EntryList* cachedEntries;
};

#endif // ONLINETRACKER_H
