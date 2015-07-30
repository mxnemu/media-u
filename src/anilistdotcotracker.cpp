#include "anilistdotcotracker.h"
#include "anilistdotcocredentials.h"
#include <QUrl>
#include <QDebug>

AnilistDotCoTracker::AnilistDotCoTracker(OnlineCredentials& credentials, QObject *parent) :
    OnlineTracker(credentials, parent)
{
}

AnilistDotCoTracker::~AnilistDotCoTracker() {

}

const QString AnilistDotCoTracker::identifierKey() const {
    return AnilistDotCoCredentials::IDENTIFIER_KEY;
}

OnlineTracker::EntryList* AnilistDotCoTracker::fetchRemote() {
    if (this->user.id <= 0) {
        if (!this->user.fetchCurrentlyLoggedInUser(this->credentials)) {
            return NULL;
        }
    }

    QUrl url(QString("https://anilist.co/api/user/%1/animelist").arg(user.id));
    CurlResult userData(NULL);

    CURL* handle = credentials.curlClientNoLock(url.toString(QUrl::FullyEncoded).toStdString().c_str(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for anilist.co Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        userData.print();
        nw::JsonReader jr(userData.data);
        EntryList* entries = new EntryList(jr);
        jr.close();
        if (!jr.getErrorMessage().empty()) {
            qDebug() << "got error from anilist.co status list parsing:" << QString(jr.getErrorMessage().c_str());
            delete entries;
            return NULL;
        }
        return entries;
    }
    return NULL;
}

OnlineTracker::UpdateResult AnilistDotCoTracker::updateinOnlineTrackerOrAdd(const TvShow *show, const QString &type) const {
    Entry e;
    e.anime.id = show->getRemoteId(identifierKey());
    e.episodes_watched = show->episodeList().highestWatchedEpisodeNumber(0);
    e.rewatched = show->getRewatchCount();
    QUrl url = (QStringList()
        << "https://anilist.co/api/animelist"
        << "?id=" << QString::number(e.anime.id)
        << "&list_status=" << watchStatusToString(show->getStatus())
//        << "&score_raw=" << e->score_raw
        << "&episodes_watched=" << QString::number(e.watchedEpisodes())
        << "&rewatched=" << QString::number(e.rewatched)).join("");
//        << "&score=" << e->score // (See bottom of page - List score types)
//        << "&notes=" << e->notes
//        << "&advanced_rating_scores=" << e->advanced_rating_scores
//        << "&custom_lists=" << e->custom_lists
//        << "&hidden_default=" << e->hidden_default

//    qDebug() << url.toString(QUrl::FullyEncoded);
//    qDebug() << show->episodeList().numberOfEpisodes() << "/"
//             << show->episodeList().highestWatchedEpisodeNumber(0)
//             << TvShow::watchStatusToString(show->getStatus())
//            << show->getRemoteId(identifierKey())
//            << show->name();

    CurlResult userData(NULL);
    CURL* handle = credentials.curlClientNoLock(url.toString(QUrl::FullyEncoded).toStdString().c_str(), userData);

    if (type == "add") {
//        curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
//        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 0);
//        curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, NULL);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
    } else {
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for anilist.co tracker " << type << " with this message:\n";
        userData.print();
        return OnlineTracker::failedDueToNetwork;
    } else {
        qDebug() << "success" << error << "for anilist.co tracker " << type << " with this message:\n";
        userData.print();
        return OnlineTracker::success;
    }
}


AnilistDotCoTracker::User::User() :
    id(-1)
{
}

bool AnilistDotCoTracker::User::fetchCurrentlyLoggedInUser(const OnlineCredentials& credentials) {
    CurlResult userData(NULL);

    CURL* handle = credentials.curlClientNoLock("https://anilist.co/api/user", userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for anilist.co user fetch with this message:\n";
        userData.print();
    } else {
        userData.print();
        nw::JsonReader jr(userData.data);
        this->describe(jr);
        jr.close();
    }
    return this->id > 0;
}

void AnilistDotCoTracker::User::describe(nw::Describer &de) {
    NwUtils::describe(de, "id", id);
    NwUtils::describe(de, "display_name", display_name);
    NwUtils::describe(de, "anime_time", anime_time);
    NwUtils::describe(de, "manga_chap", manga_chap);
    NwUtils::describe(de, "about", about);
    NwUtils::describe(de, "list_order", list_order);
    NwUtils::describe(de, "adult_content", adult_content);
    NwUtils::describe(de, "following", following);
    NwUtils::describe(de, "image_url_lge", image_url_lge);
    NwUtils::describe(de, "image_url_med", image_url_med);
    NwUtils::describe(de, "image_url_banner", image_url_banner);
    NwUtils::describe(de, "title_language", title_language);
    NwUtils::describe(de, "score_type", score_type);
    NwUtils::describeValueArray(de, "custom_list_anime", custom_list_anime);
    NwUtils::describeValueArray(de, "custom_list_manga", custom_list_manga);
    NwUtils::describeValueArray(de, "advanced_rating", advanced_rating);
    NwUtils::describeValueArray(de, "advanced_rating_names", advanced_rating_names);
    NwUtils::describe(de, "notifications", notifications);
}

AnilistDotCoTracker::EntryList::EntryList(nw::JsonReader &jr) {
    this->describe(jr);
}

const OnlineTracker::Entry *AnilistDotCoTracker::EntryList::get(const QString trackerIdentifierKey, const TvShow *show) const {
    int id = show->getRemoteId(trackerIdentifierKey);
    foreach (const Entry& item, entries) {
        if (item.remoteId() == id) {
            return &item;
        }
    }
    return NULL;
}

void AnilistDotCoTracker::EntryList::describe(nw::Describer &de)
{
// "lists":{"completed":[{
    if (de.getErrorMessage().length()) {
        qDebug() << de.getErrorMessage().c_str();
    }
    if (de.push("lists")) {
    // TODO iterate over all keys available
        this->describeList(de, "completed");
        this->describeList(de, "dropped");
        this->describeList(de, "on_hold");
        this->describeList(de, "plan_to_watch");
        this->describeList(de, "watching");
//        this->describeList(de, "");
        de.pop();
    }
}

void AnilistDotCoTracker::EntryList::describeList(nw::Describer &de, nw::String listKey) {
    de.describeArray(listKey, "", this->entries.length());
    for (int i=0; de.enterNextElement(i); ++i) {
        Entry e;
        e.describe(de);
        this->entries.push_back(e);
    }
}


void AnilistDotCoTracker::Entry::describe(nw::Describer &de) {
    if (de.push("anime")) {
        anime.describe(de);
        de.pop();
    }

    NwUtils::describe(de, "list_status", list_status);
    NwUtils::describe(de, "score", score);
    NwUtils::describe(de, "priorty", priorty);
    NwUtils::describe(de, "rewatched", rewatched);
    NwUtils::describe(de, "notes", notes);
    NwUtils::describe(de, "private", isPrivate);
    NwUtils::describe(de, "updated_time", updated_time);
    NwUtils::describe(de, "added_time", added_time);
    NwUtils::describe(de, "score_raw", score_raw);
    NwUtils::describeValueArray(de, "advanced_rating_scores", advanced_rating_scores);
    NwUtils::describe(de, "episodes_watched", episodes_watched);
    NwUtils::describe(de, "chapters_read", chapters_read);
    NwUtils::describe(de, "volumes_read", volumes_read);
    NwUtils::describe(de, "hidden_default", hidden_default);
    NwUtils::describeValueArray(de, "custom_lists", custom_lists);
}

QString AnilistDotCoTracker::watchStatusToString(TvShow::WatchStatus status) {
    if (status == TvShow::completed) return "completed";
    if (status == TvShow::watching) return "watching";
    if (status == TvShow::waitingForNewEpisodes) return "watching";
    if (status == TvShow::onHold) return "on-hold";
    if (status == TvShow::dropped) return "dropped";
    if (status == TvShow::planToWatch) return "plan to watch";
    return "watching";
}

TvShow::WatchStatus AnilistDotCoTracker::watchStatusFromString(QString status) {
    if (status == "watching") return TvShow::watching;
    if (status == "completed") return TvShow::completed;
    if (status == "on-hold") return TvShow::onHold;
    if (status == "dropped") return TvShow::dropped;
    if (status == "plan to watch") return TvShow::planToWatch;
    return TvShow::planToWatch;
}

TvShow::WatchStatus AnilistDotCoTracker::Entry::calculateWatchStatus(TvShow::WatchStatus status) const {
    if (status == TvShow::waitingForNewEpisodes) {
        return TvShow::watching;
    }
    return status;
}

bool AnilistDotCoTracker::Entry::remoteIsEq(const TvShow *show) const {
    const TvShow::WatchStatus status = show->getStatus();
    const TvShow::WatchStatus statusWouldSendIfSynced = calculateWatchStatus(status);
    const TvShow::WatchStatus entryStatus = watchStatusFromString(this->list_status);
    // allow anilist.co to claim completion, when unseparated OVAs are not watched, yet. Take it as up2date.
    const bool statusUpToDate =
            statusWouldSendIfSynced == entryStatus ||
            (entryStatus == TvShow::completed && statusWouldSendIfSynced == TvShow::watching);

    const bool episodesUpToDate =
            this->watchedEpisodes() >=
            std::min(this->anime.total_episodes, (int)show->episodeList().highestWatchedEpisodeNumber(0));

//    const bool rewatchUpToDate =
//            my_rewatching >= show->getRewatchCount() &&
//            my_rewatching_ep >= show->getRewatchMarker();

    return  statusUpToDate && episodesUpToDate/*&& rewatchUpToDate*/;
}
