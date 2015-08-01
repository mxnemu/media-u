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
        virtual QDateTime lastUpdate() const = 0;
        virtual int remoteId() const = 0;
        virtual int watchedEpisodes() const = 0;
        virtual int rewatchMarker() const = 0;
        virtual int rewatchCount() const = 0;
        virtual int totalEpisodes() const = 0;

        virtual TvShow::WatchStatus getStatusWouldSendIfSynced(TvShow::WatchStatus showStatus) const = 0;
        virtual TvShow::WatchStatus watchStatus() const = 0;

        bool localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const;
        bool remoteIsUpToDate(const TvShow* show) const;
        bool syncConflict(const QString trackerIdentifier, const TvShow* show) const;
        void updateShow(const QString trackerIdentifierKey, TvShow *show) const;

        virtual bool supportsRewatchMarker() const = 0;
        virtual bool remoteIsEq(const TvShow* show) const; ///< checks data not dates
    };

    class EntryList {
    public:
        EntryList();
        virtual ~EntryList();
        virtual const Entry* get(const QString trackerIdentifierKey, const TvShow* show) const = 0;
        virtual void makeSureLocalIsUpdated(const QString trackerIdentifierKey, TvShow* show) const;
        virtual void describe(nw::Describer& de) = 0;

        bool tooOld() const;
        const QDateTime fetchTime;
    };

    explicit OnlineTracker(const OnlineCredentials& credentials, OnlineCredentials::TimeLock& lock, QObject *parent = 0);
    virtual ~OnlineTracker();
    bool updateRemote(TvShow* show);
    virtual EntryList* fetchRemote() = 0; ///< Must create a new EntryList using online Data, or NULL on error
    virtual const QString identifierKey() const = 0;

    const OnlineCredentials& credentials;

protected:
    virtual UpdateResult updateinOnlineTrackerOrAdd(const TvShow* show, const QString& type) const = 0;

    OnlineCredentials::TimeLock& lock;

private:
    /// Either returns cached entries, or fetches new ones when cached are too old
    EntryList* satisfyingEntries();
    UpdateResult updateRemoteImpl(const TvShow* show, const EntryList& entries) const;

    EntryList* cachedEntries;

    static QDebug log();
    static QDebug err();
};

#endif // ONLINETRACKER_H
