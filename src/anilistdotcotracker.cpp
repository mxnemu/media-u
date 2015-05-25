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
//    Entry* e = this->entries.get(show);
//    QStringList()
//        << "?id=" << e->anime.id
//        << "&list_status=" << status()
//        //<< "score " score: (See bottom of page - List score types)
//        << "&score_raw=" << e->score
//    episodes_watched: (int)
//    rewatched: (int)
//    notes: (String)
//    advanced_rating_scores: comma separated scores, same order as advanced_rating_names
//    custom_lists: comma separated 1 or 0, same order as custom_list_anime
//    hidden_default: (int) 0 || 1

    return OnlineTracker::invalid;
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

bool AnilistDotCoTracker::User::remoteIsEq(const TvShow *show) const {
    return false;
}

AnilistDotCoTracker::EntryList::EntryList(nw::JsonReader &jr) {
    this->describe(jr);
}

const OnlineTracker::Entry *AnilistDotCoTracker::EntryList::get(const QString trackerIdentifierKey, const TvShow *show) const
{
    return NULL;
}

void AnilistDotCoTracker::EntryList::makeSureLocalIsUpdated(const QString trackerIdentifierKey, TvShow *show) const
{

}

void AnilistDotCoTracker::EntryList::describe(nw::Describer &de)
{
// "lists":{"":[{
    de.describeArray("lists", "", this->entries.length());
    for (int i=0; de.enterNextElement(i); ++i) {
        Entry e;
        e.describe(de);
        this->entries.push_back(e);
    }
}


void AnilistDotCoTracker::Entry::describe(nw::Describer &de) {
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
    anime.describe(de);
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

bool AnilistDotCoTracker::Entry::remoteIsEq(const TvShow *show) const {
    TvShow::WatchStatus status = show->getStatus();
//    TvShow::WatchStatus statusWouldSendIfSynced = restoreStatus(UpdateItem::calculateWatchStatus(status));
    // allow mal to claim completion, when unseparated OVAs are not watched, yet. Take it as up2date.
//    const bool statusUpToDate =
//            statusMalWouldSendIfSynced == this->list_status ||
//            (this->my_status == TvShow::completed && statusMalWouldSendIfSynced == TvShow::watching);

    const bool episodesUpToDate =
            this->watched_episodes >=
            std::min(this->anime.total_episodes, (int)show->episodeList().highestWatchedEpisodeNumber(0));

//    const bool rewatchUpToDate =
//            my_rewatching >= show->getRewatchCount() &&
//            my_rewatching_ep >= show->getRewatchMarker();

    return  /*statusUpToDate &&*/ episodesUpToDate/*&& rewatchUpToDate*/;
}
