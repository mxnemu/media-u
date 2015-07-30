#include "onlinetracker.h"
#include <QDebug>

OnlineTracker::OnlineTracker(const OnlineCredentials& credentials, QObject *parent) :
    QObject(parent),
    credentials(credentials),
    cachedEntries(NULL)
{
}

OnlineTracker::~OnlineTracker() {
    if (this->cachedEntries) {
        delete this->cachedEntries;
        this->cachedEntries = NULL;
    }
}

bool OnlineTracker::updateRemote(TvShow* show) {
    OnlineTracker::EntryList* entries = satisfyingEntries();
    if (!entries) {
        return false;
    }

    entries->makeSureLocalIsUpdated(this->identifierKey(), show);

    OnlineTracker::UpdateResult result = this->updateRemoteImpl(show, *entries);
    switch (result) {
    case OnlineTracker::success:
        show->setLastOnlineTrackerUpdate(this->identifierKey(), QDateTime::currentDateTimeUtc());
        log() << "update success" << show->name() << identifierKey();
        return true;
    case OnlineTracker::alreadySameAsLocal: {
        const OnlineTracker::Entry* entry = entries->get(this->identifierKey(), show);
        if (entry) {
            show->setLastOnlineTrackerUpdate(this->identifierKey(), entry->lastUpdate());
        }
        log() << "skip alreadySameAsLocal" << show->name() << identifierKey();
        return true;
    }
    case OnlineTracker::skipDueToNoChanges:
        log() << "skip up2date" << show->name() << identifierKey();
        return true;
    case OnlineTracker::invalid:
    case OnlineTracker::failedDueToMissingData:
    case OnlineTracker::failedDueToNetwork:
    default:
        log() << "update failure" << show->name() << result << identifierKey();
        return false;
    }
}


OnlineTracker::UpdateResult OnlineTracker::updateRemoteImpl(const TvShow* show, const OnlineTracker::EntryList& e) const {
    int id = show->getRemoteId(identifierKey());
    if (id <= 0) return failedDueToMissingData;

//    if (this->e == NULL) {
//        return failedDueToMissingData;
//    }

    const Entry* item = e.get(this->identifierKey(), show);
    if (item) {
        if (item->localIsUpToDate(this->identifierKey(), show) && !item->remoteIsUpToDate(show)) {
            if (item->remoteIsEq(show)) {
                return alreadySameAsLocal;
            }
            return this->updateinOnlineTrackerOrAdd(show, "update");
        }
        return skipDueToNoChanges;
    } else {
        return this->updateinOnlineTrackerOrAdd(show, "add");
    }
}


OnlineTracker::EntryList*OnlineTracker::satisfyingEntries() {
    if (!cachedEntries || cachedEntries->tooOld()) {
        this->cachedEntries = fetchRemote();
    }
    return this->cachedEntries;
}

OnlineTracker::EntryList::EntryList() :
    fetchTime(QDateTime::currentDateTimeUtc())
{
}

OnlineTracker::EntryList::~EntryList() {}

void OnlineTracker::EntryList::makeSureLocalIsUpdated(const QString trackerIdentifierKey, TvShow *show) const {
    const Entry* item = this->get(trackerIdentifierKey, show);
    if (item) {
        item->updateShow(trackerIdentifierKey, show);
    }
}

void OnlineTracker::Entry::updateShow(const QString trackerIdentifierKey, TvShow* show) const {
    if (!localIsUpToDate(trackerIdentifierKey, show)) {
        int marker = this->rewatchMarker() == 0 ? -1 : this->rewatchMarker();
        int count = this->rewatchCount();
        if (syncConflict(trackerIdentifierKey, show)) {
            show->episodeList().setMinimalWatched(this->watchedEpisodes());
            marker = std::max(marker, show->getRewatchMarker());
            count = std::max(count, show->getRewatchCount());
        } else {
            show->episodeList().setMaximalWatched(this->watchedEpisodes());
        }
        show->setRewatchCount(count, false);
        if (this->supportsRewatchMarker()) {
            show->setRewatchMarker(marker, false);
        }
        show->setLastOnlineTrackerUpdate(trackerIdentifierKey, this->lastUpdate());
    }
    if (show->getLastOnlineTrackerUpdate(trackerIdentifierKey).isNull()) {
        show->setLastOnlineTrackerUpdate(trackerIdentifierKey, this->lastUpdate());
    }
}


bool OnlineTracker::Entry::remoteIsEq(const TvShow *show) const {
    const TvShow::WatchStatus status = show->getStatus();
    const TvShow::WatchStatus statusWouldSendIfSynced = this->getStatusWouldSendIfSynced(status);
    const TvShow::WatchStatus entryStatus = this->watchStatus();
    // allow remote to claim completion, when unseparated OVAs are not watched, yet. Take it as up2date.
    const bool statusUpToDate =
            statusWouldSendIfSynced == entryStatus ||
            (entryStatus == TvShow::completed && statusWouldSendIfSynced == TvShow::watching);

    const bool episodesUpToDate =
            this->watchedEpisodes() >=
            std::min(this->totalEpisodes(), (int)show->episodeList().highestWatchedEpisodeNumber(0));

    const bool rewatchUpToDate =
            rewatchCount() >= show->getRewatchCount() &&
            (this->supportsRewatchMarker() ? rewatchMarker() >= show->getRewatchMarker() : true);

    return  statusUpToDate && episodesUpToDate && rewatchUpToDate;
}

bool OnlineTracker::EntryList::tooOld() const {
    return this->fetchTime.addSecs(60 * 5) < QDateTime::currentDateTimeUtc();
}


bool OnlineTracker::Entry::syncConflict(const QString trackerIdentifier, const TvShow* show) const {
    return show->getLastLocalUpdate() > lastUpdate() &&
            lastUpdate() > show->getLastOnlineTrackerUpdate(trackerIdentifier);
}

OnlineTracker::Entry::~Entry() {}

bool OnlineTracker::Entry::localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const {
    const QDateTime lastLocalUpdate = show->getLastOnlineTrackerUpdate(trackerIdentifier);
    if (lastLocalUpdate.isNull()) {
        return show->episodeList().highestWatchedEpisodeNumber(0) >= this->watchedEpisodes();
    }
    bool localIsNew = lastLocalUpdate >= this->lastUpdate();
    return localIsNew;
}

bool OnlineTracker::Entry::remoteIsUpToDate(const TvShow* show) const {
    const QDateTime lastLocalUpdate = show->getLastLocalUpdate();
    return (!lastLocalUpdate.isNull() &&
            (!this->lastUpdate().isNull() && this->lastUpdate() >= lastLocalUpdate));
}

QDebug OnlineTracker::log() {
    return qDebug() << "TRACKER";
}
QDebug OnlineTracker::err() {
    return qDebug() << "TRACKER";
}

