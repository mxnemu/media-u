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
        return true;
    case OnlineTracker::alreadySameAsLocal: {
        const OnlineTracker::Entry* entry = entries->get(this->identifierKey(), show);
        if (entry) {
            show->setLastOnlineTrackerUpdate(this->identifierKey(), entry->lastUpdate);
        }
        return true;
    }
    case OnlineTracker::skipDueToNoChanges:
        return true;
    case OnlineTracker::invalid:
    case OnlineTracker::failedDueToMissingData:
    case OnlineTracker::failedDueToNetwork:
    default:
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
        qDebug() << "MAL TRACKER skip up2date" << show->name();
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

bool OnlineTracker::EntryList::tooOld() const {
    return this->fetchTime.addSecs(60 * 5) < QDateTime::currentDateTimeUtc();
}


bool OnlineTracker::Entry::syncConflict(const QString trackerIdentifier, const TvShow* show) const {
    return show->getLastLocalUpdate() > lastUpdate &&
            lastUpdate > show->getLastOnlineTrackerUpdate(trackerIdentifier);
}

OnlineTracker::Entry::~Entry() {}

bool OnlineTracker::Entry::localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const {
    const QDateTime lastLocalUpdate = show->getLastOnlineTrackerUpdate(trackerIdentifier);
    if (lastLocalUpdate.isNull()) {
        return show->episodeList().highestWatchedEpisodeNumber(0) >= this->watchedEpisodes;
    }
    bool localIsNew = lastLocalUpdate >= this->lastUpdate;
    return localIsNew;
}

bool OnlineTracker::Entry::remoteIsUpToDate(const TvShow* show) const {
    const QDateTime lastLocalUpdate = show->getLastLocalUpdate();
    return (!lastLocalUpdate.isNull() &&
            (!this->lastUpdate.isNull() && this->lastUpdate >= lastLocalUpdate));
}
