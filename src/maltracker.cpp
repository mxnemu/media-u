#include "maltracker.h"
#include <QUrl>
#include <QDebug>

namespace Mal {

Tracker::Tracker(const OnlineCredentials& credentials, QObject *parent) :
    OnlineTracker(credentials, parent)
{
}

CURL* Tracker::curlTrackerUpdateClient(const char* url, CurlResult& userdata, UpdateItem& data) const {
    CURL* handle = credentials.curlClientNoLock(url, userdata);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
    QString dataStr = QUrl(data.toXml()).toEncoded();
    QByteArray xml = QString("data=%1").arg(dataStr).toUtf8();
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, xml.size());
    curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, xml.data());
    return handle;
}

OnlineTracker::EntryList* Tracker::fetchRemote() const {
    QString url = QString("http://myanimelist.net/malappinfo.php?u=%1&status=all&type=anime").arg(credentials.getUsername());
    CurlResult userData(NULL);

    CURL* handle = credentials.curlNoAuthClientNoLock(url.toUtf8().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        nw::XmlReader xr(userData.data);

        EntryList* entries = new EntryList(xr);
        if (!entries->error.isEmpty()) {
            qDebug() << "got error from mal status list fetching:" << entries->error;
            delete entries;
            return NULL;
        }
        return entries;
    }
    return NULL;
}

const QString Tracker::IDENTIFIER_KEY = "mal";
const QString Tracker::identifierKey() const {
    return IDENTIFIER_KEY;
}

OnlineTracker::UpdateResult Tracker::updateRemoteImpl(const TvShow* show, const OnlineTracker::EntryList& e) const {
    int id = show->getRemoteId(identifierKey());
    if (id <= 0) return failedDueToMissingData;

    const EntryList& entries = static_cast<const EntryList&>(e);
    if (!entries.error.isEmpty()) {
        return failedDueToMissingData;
    }

    const Entry* item = entries.get(this->identifierKey(), show);
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

OnlineTracker::UpdateResult Tracker::updateinOnlineTrackerOrAdd(const TvShow* show, const QString& type) const {
    QString url = QString("http://myanimelist.net/api/animelist/%2/%1.xml").arg(QString::number(show->getRemoteId(identifierKey())), type);
    CurlResult userData;
    UpdateItem updateData(show);

    CURL* handle = curlTrackerUpdateClient(url.toUtf8().data(), userData, updateData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        if (type == "update" && userData.data.str() == "Updated") {
            qDebug() << "MAL TRACKER UPDATE success" << show->name();
            return success;
        } else if (type == "add") {
            QString responseString = userData.data.str().c_str();
            if (responseString.contains("201 Created")) {
                qDebug() << "MAL TRACKER ADD success" << show->name() << QDateTime::currentDateTimeUtc();
                return success;
            }
        }
    }
    qDebug() << "Could not" << type << "MAL tracker:\n";
    userData.print();
    return failedDueToNetwork;
}


UpdateItem::UpdateItem(const TvShow* show) {
    this->episode = show->episodeList().highestWatchedEpisodeNumber();
    this->status = calculateWatchStatus(show->getStatus());
    this->downloaded_episodes = show->episodeList().numberOfEpisodes();
    this->times_rewatched = std::max(0, show->getRewatchCount());
    this->rewatch_value = std::max(0, show->getRewatchMarker());

    score = -1;
    storage_type = -1; // int (will be updated to accomodate strings soon) // yeah sure soon...
    storage_value = -1; // wat
    priority = -1; // 0 - 10 ? dont know didn't check
    enable_discussion = 0; // int. 1=enable, 0=disable
    // TODO add a status for this
    enable_rewatching = -1; // int. 1=enable, 0=disable
    fansub_group = show->favouriteReleaseGroup();
    QStringList tags; // string. tags separated by commas
}

UpdateWatchStatus UpdateItem::calculateWatchStatus(const TvShow::WatchStatus status) {
    switch (status) {
    case TvShow::waitingForNewEpisodes:
    case TvShow::watching:
        return watching;
    case TvShow::completed: return completed;
    case TvShow::onHold: return onhold;
    case TvShow::dropped: return dropped;
    case TvShow::planToWatch:
    default:
        return plantowatch;
    }
}

void UpdateItem::describe(nw::Describer& de) {
    int statusInt = status;
    nw::String empty = "";
    NwUtils::describe(de, "episode", episode);
    NwUtils::describe(de, "status", statusInt);
    NwUtils::describe(de, "score", empty);
    NwUtils::describe(de, "downloaded_episodes", downloaded_episodes);
    NwUtils::describe(de, "storage_type", empty);
    NwUtils::describe(de, "storage_value", empty);
    NwUtils::describe(de, "times_rewatched", times_rewatched);
    NwUtils::describe(de, "rewatch_value", rewatch_value);
    NwUtils::describe(de, "date_start", empty);
    NwUtils::describe(de, "date_finish", empty);
    NwUtils::describe(de, "priority", empty);
    NwUtils::describe(de, "enable_discussion", enable_discussion);
    NwUtils::describe(de, "enable_rewatching", empty);
    NwUtils::describe(de, "comments", empty);
    NwUtils::describe(de, "fansub_group", fansub_group);
    NwUtils::describe(de, "tags", tags, ',');
}

QString UpdateItem::toXml() {
    std::stringstream ss;
    nw::XmlWriter xw(ss);
    this->describe(xw);

    nw::Tag* tag = xw.getMotherTag();
    tag->setName("entry");
    tag->setCanBeAttributeRecursive(false);
    xw.close();
    return QString(ss.str().data());
}

TvShow::WatchStatus Tracker::Entry::restoreStatus(int malStatusId) {
    switch(malStatusId) {
    case 1: return TvShow::watching;
    case 2: return TvShow::completed;
    case 3: return TvShow::onHold;
    case 4: return TvShow::dropped;
    case 6:
    default:
        return TvShow::planToWatch;
    }
}


Tracker::EntryList::EntryList() :
    error("noinit")
{
}

Tracker::EntryList::~EntryList() {

}

Tracker::EntryList::EntryList(nw::Describer& de) {
    describe(de);
}

void Tracker::EntryList::updateShows(const QString trackerIdentifierKey, QList<TvShow*> shows) {
    foreach (Entry* item, items) {
        foreach (TvShow* show, shows) {
            if (show->getRemoteId(trackerIdentifierKey) == item->series_animedb_id) {
                item->updateShow(trackerIdentifierKey, show);
                break;
            }
        }
    }
}

const Tracker::Entry* Tracker::EntryList::get(const QString trackerIdentifierKey, const TvShow* show) const {
    int id = show->getRemoteId(trackerIdentifierKey);
    foreach (const Tracker::Entry* item, items) {
        if (item->series_animedb_id == id) {
            return item;
        }
    }
    return NULL;
}

void Tracker::EntryList::describe(nw::Describer& de) {
    error.clear();
    NwUtils::describe(de, "error", error);
    if (!error.isEmpty()) {
        return;
    }
    de.describeArray("", "anime", -1);
    for (int i=0; de.enterNextElement(i); ++i) {
        items.push_back(new Entry(de));
    }
}

Tracker::Entry::Entry(nw::Describer& de) {
    describe(de);
}

void Tracker::Entry::describe(nw::Describer& de) {
    int status = -1;
    NwUtils::describe(de, "series_animedb_id", series_animedb_id);
    NwUtils::describe(de, "series_title", series_title);
    NwUtils::describe(de, "series_synonyms", series_synonyms);
    NwUtils::describe(de, "series_type", series_type);
    NwUtils::describe(de, "series_episodes", series_episodes);
    NwUtils::describe(de, "series_status", series_status);
    NwUtils::describe(de, "series_start", series_start); //2004-10-05
    NwUtils::describe(de, "series_end", series_end);
    NwUtils::describe(de, "series_image", series_image);
    //QString my_id; // always 0 no idea what it does
    NwUtils::describe(de, "my_watched_episodes", my_watched_episodes);
    //QDate my_start_date; // 0000-00-00
    //QDate my_finish_date; // 0000-00-00
    NwUtils::describe(de, "my_score", my_score);
    NwUtils::describe(de, "my_status", status);
    NwUtils::describe(de, "my_rewatching", my_rewatching);
    NwUtils::describe(de, "my_rewatching_ep", my_rewatching_ep);

    uint unixTimeUpdate = 0; // unix time int example: 1388944557
    NwUtils::describe(de, "my_last_updated", unixTimeUpdate);
    this->lastUpdate = QDateTime::fromTime_t(unixTimeUpdate);

    //QStringList my_tags; // separated by ", "
    my_status = Entry::restoreStatus(status);
}

void Tracker::Entry::updateShow(const QString trackerIdentifierKey, TvShow* show) {
    if (!localIsUpToDate(trackerIdentifierKey, show)) {
        int marker = this->my_rewatching_ep == 0 ? -1 :this->my_rewatching_ep;
        int count = this->my_rewatching;
        if (syncConflict(trackerIdentifierKey, show)) {
            show->episodeList().setMinimalWatched(this->my_watched_episodes);
            marker = std::max(marker, show->getRewatchMarker());
            count = std::max(this->my_rewatching, show->getRewatchCount());
        } else {
            show->episodeList().setMaximalWatched(this->my_watched_episodes);
        }
        show->setRewatchCount(count, false);
        show->setRewatchMarker(marker, false);
        show->setLastOnlineTrackerUpdate(trackerIdentifierKey, this->lastUpdate);
    }
    if (show->getLastOnlineTrackerUpdate(trackerIdentifierKey).isNull()) {
        show->setLastOnlineTrackerUpdate(trackerIdentifierKey, this->lastUpdate);
    }
}

bool Tracker::Entry::syncConflict(const QString trackerIdentifier, const TvShow* show) const {
    return show->getLastLocalUpdate() > lastUpdate &&
            lastUpdate > show->getLastOnlineTrackerUpdate(trackerIdentifier);
}

bool Tracker::Entry::localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const {
    if (show->getLastOnlineTrackerUpdate(trackerIdentifier).isNull()) {
        return show->episodeList().highestWatchedEpisodeNumber(0) >= this->my_watched_episodes;
    }
    return show->getLastOnlineTrackerUpdate(trackerIdentifier) >= this->lastUpdate;
}

bool Tracker::Entry::remoteIsUpToDate(const TvShow* show) const {
    const QDateTime lastLocalUpdate = show->getLastLocalUpdate();
    return (!lastLocalUpdate.isNull() &&
            (!lastUpdate.isNull() && lastUpdate >= lastLocalUpdate));
}

bool Tracker::Entry::remoteIsEq(const TvShow* show) const {
    TvShow::WatchStatus status = show->getStatus();
    TvShow::WatchStatus statusMalWouldSendIfSynced = restoreStatus(UpdateItem::calculateWatchStatus(status));
    // allow mal to claim completion, when unseparated OVAs are not watched, yet. Take it as up2date.
    const bool statusUpToDate =
            statusMalWouldSendIfSynced == this->my_status ||
            (this->my_status == TvShow::completed && statusMalWouldSendIfSynced == TvShow::watching);

    const bool episodesUpToDate =
            this->my_watched_episodes >=
            std::min(this->series_episodes, (int)show->episodeList().highestWatchedEpisodeNumber(0));

    const bool rewatchUpToDate =
            my_rewatching >= show->getRewatchCount() &&
            my_rewatching_ep >= show->getRewatchMarker();

    return  statusUpToDate && episodesUpToDate && rewatchUpToDate;
}


}
