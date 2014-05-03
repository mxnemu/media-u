#include "onlinetracker.h"

OnlineTracker::OnlineTracker(const OnlineCredentials& credentials, QObject *parent) :
    QObject(parent),
    credentials(credentials)
{
}

bool OnlineTracker::updateRemote(TvShow* show) {
    OnlineTracker::UpdateResult result = this->updateRemoteImpl(show);
    switch (result) {
    case OnlineTracker::success:
        show->setLastOnlineTrackerUpdate(this->identifierKey(), QDateTime::currentDateTimeUtc());
        return true;
    case OnlineTracker::alreadySameAsLocal: {
        OnlineTracker::EntryList* entries = this->getEntries();
        if (entries) {
            const OnlineTracker::Entry* entry = entries->get(this->identifierKey(), show);
            if (entry) {
                show->setLastOnlineTrackerUpdate(this->identifierKey(), entry->lastUpdate);
            }
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


OnlineTracker::EntryList*OnlineTracker::getEntries() {
    return this->entries;
}
