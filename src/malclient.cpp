#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include <QFile>
#include "nwutils.h"
#include "utils.h"

namespace Mal {

Client::Client(QObject *parent) :
    OnlineTvShowDatabase::Client(parent),
    userAgent("nemu-malapiclient"),
    activeThread(NULL)
{
    mHasVerifiedCredentials = false;
}

void Client::init(QString configFilePath) {
    if (QFile(configFilePath).exists()) {
        std::string user, password;

        nw::JsonReader jr(configFilePath.toStdString());
        jr.describe("user", user);
        jr.describe("password", password);
        NwUtils::describe(jr, "userAgent", userAgent);
        jr.close();

        if (user.length() > 0 && password.length() > 0) {
            this->setCredentials(QString(user.data()), QString(password.data()));
        }
    }
}

void Client::fetchShows(QList<TvShow*> &showList, const Library& library) {
    if (activeThread) {
        return;
    }
    activeThread = new Thread(*this, showList, library, this);
    activeThread->start(QThread::LowPriority);
    qDebug() << "started mal fetchThread";

    connect(activeThread, SIGNAL(finished()),
            this, SLOT(fetchThreadFinished()));
}

void Client::fetchThreadFinished() {
    if (activeThread == sender()) {
        delete activeThread;
        this->activeThread = NULL;
    } else {
        throw "fetchThreadFinished() signal received from unknown sender()";
    }
    emit fetchingFinished();
}

OnlineTvShowDatabase::SearchResult* Client::search(QString anime) {
    QString name = anime;
    if (name.isEmpty() || name.isNull()) {
        return NULL;
    }

    QString url = "http://myanimelist.net/api/anime/search.xml?q=";
    url.append(name.replace(' ', '+').remove('~'));

    CurlResult userData(this);
    CURL* handle = curlClient(QUrl(url).toString(QUrl::FullyEncoded).toLocal8Bit().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        return new SearchResult(userData, name);
    }
    return NULL;
}

const OnlineTvShowDatabase::Entry*Client::bestResult(const OnlineTvShowDatabase::SearchResult& result) const {
    return ((SearchResult&)result).bestResult(); // not a good cast use pointers
}



void Client::setCredentials(const QString name, const QString password) {
    this->username = name;
    this->password = password;
}

bool Client::verifyCredentials() {
    if (username.length() <= 0 || password.length() <= 0) {
        return false;
    }

    CurlResult userData(this);
    CURL* handle = curlClient("http://myanimelist.net/api/account/verify_credentials.xml", userData);
    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error " << error << " with this message:\n";
        userData.print();
    } else {
        if (userData.data.str() == "Invalid credentials") {
            mHasVerifiedCredentials = false;
        } else {
            mHasVerifiedCredentials = true;
        }
    }

    qDebug() << "mal connection is " << mHasVerifiedCredentials;
    curl_easy_cleanup(handle);
    return mHasVerifiedCredentials;
}

CURL* Client::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curlNoAuthClient(url, userdata);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    return handle;
}

CURL*Client::curlNoAuthClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, userAgent.toLatin1().data());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

CURL* Client::curlTrackerUpdateClient(const char* url, CurlResult& userdata, AnimeUpdateData& data) {
    CURL* handle = curlClient(url, userdata);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
    QString dataStr = QUrl(data.toXml()).toEncoded();
    QByteArray xml = QString("data=%1").arg(dataStr).toUtf8();
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, xml.size());
    curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, xml.data());
    return handle;
}



bool Client::hasVerifiedCredentials() const {
    return mHasVerifiedCredentials;
}

bool Client::login() {
    return this->hasVerifiedCredentials() || this->verifyCredentials();
}

bool Client::fetchOnlineTrackerList(QList<TvShow*>& shows) {

    QString url = QString("http://myanimelist.net/malappinfo.php?u=%1&status=all&type=anime").arg(username);
    CurlResult userData(this);

    CURL* handle = curlNoAuthClient(url.toUtf8().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        nw::XmlReader xr(userData.data);
        animeListData.describe(xr);
        if (!animeListData.error.isEmpty()) {
            qDebug() << "got error from mal status list fetching:" << animeListData.error;
            return false;
        }
        animeListData.updateShows(shows);
        return true;
    }
    return false;
}

bool Client::updateInOnlineTracker(TvShow* show) {
    int id = show->getRemoteId();
    if (id <= 0) return false;

    if (!animeListData.error.isEmpty()) {
        return false;
    }

    const AnimeItemData* item = animeListData.getShow(show);
    if (item) {
        if (item->localIsUpToDate(show) && !item->remoteIsUpToDate(show)) {
            return this->updateinOnlineTrackerOrAdd(show, "update");
        }
        qDebug() << "MAL TRACKER skip up2date" << show->name();
        return true;
    } else {
        return this->updateinOnlineTrackerOrAdd(show, "add");
    }
}

bool Client::updateinOnlineTrackerOrAdd(TvShow* show, const QString& type) {
    QString url = QString("http://myanimelist.net/api/animelist/%2/%1.xml").arg(QString::number(show->getRemoteId()), type);
    CurlResult userData(this);
    AnimeUpdateData updateData(show);

    CURL* handle = curlTrackerUpdateClient(url.toUtf8().data(), userData, updateData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        if (type == "update" && userData.data.str() == "Updated") {
            show->setLastOnlineTrackerUpdate(QDateTime::currentDateTimeUtc());
            qDebug() << "MAL TRACKER UPDATE success" << show->name();
            return true;
        } else if (type == "add") {
            QString responseString = userData.data.str().c_str();
            if (responseString.contains("201 Created")) {
                show->setLastOnlineTrackerUpdate(QDateTime::currentDateTimeUtc());
                qDebug() << "MAL TRACKER ADD success" << show->name() << QDateTime::currentDateTimeUtc();
                return true;
            }
        }
    }
    qDebug() << "Could not" << type << "MAL tracker:\n";
    userData.print();
    return false;
}

///////////////////////////////////////////////////////////////////
//
// MAL ENTRY
//
//////////////////////////////////////////////////////////////////

void Entry::calculateQuerySimiliarity(const QString query) {
    int titleResult = Utils::querySimiliarity(query, title);
    int englishTitleResult = Utils::querySimiliarity(query, englishTitle);

    int bestResult = titleResult > englishTitleResult ? titleResult : englishTitleResult;

    for (int i=0; i < synonyms.length(); ++i) {
        const QString& synonym = synonyms.at(i);
        int result = Utils::querySimiliarity(query, synonym);
        if (result > bestResult) {
            bestResult = result;
        }
    }
    querySimiliarityScore = bestResult;
}

int Entry::getRemoteId() const {
    return id.toInt();
}

void Entry::updateSynopsis(TvShow& show) const {
    show.setSynopsis(synopsis);
}

void Entry::updateTitle(TvShow&) const {
    //show.setName();
//    show.setLongTitle(title);
}

void Entry::updateRemoteId(TvShow& show) const {
    show.setRemoteId(id.toInt());
}

void Entry::updateRelations(TvShow& ) const {

}

void Entry::updateAiringDates(TvShow& show) const {
    show.setTotalEpisodes(episodes);
    show.setShowType(type);
    show.setAiringStatus(status);
    show.setStartDate(QDate::fromString(startDate, Entry::dateFormat));
    show.setEndDate(QDate::fromString(endDate, Entry::dateFormat));
}

void Entry::updateSynonyms(TvShow& show) const {
    show.setSynonyms(synonyms);
}

void Entry::updateImage(TvShow& show, QDir libraryDir) const {
    show.downloadImage(image, libraryDir);
}

Entry::Entry(nw::XmlReader& reader) {
    parse(reader);
    querySimiliarityScore = 0;
}

void Entry::parse(nw::XmlReader &xr) {
    NwUtils::describe(xr, "id", id);
    NwUtils::describe(xr, "title", title);
    NwUtils::describe(xr, "englishTitle", englishTitle);
    this->parseSynonyms(xr);
    NwUtils::describe(xr, "episodes", episodes);
    NwUtils::describe(xr, "type", type);
    NwUtils::describe(xr, "status", status);
    NwUtils::describe(xr, "start_date", startDate);
    NwUtils::describe(xr, "end_date", endDate);
    NwUtils::describe(xr, "synopsis", synopsis);
    NwUtils::describe(xr, "image", image);

    title = QUrl::fromPercentEncoding(title.toLatin1());
    image = QUrl::fromPercentEncoding(image.toLatin1());
    synopsis = QUrl::fromPercentEncoding(synopsis.toLatin1());
}

void Entry::parseSynonyms(nw::XmlReader &reader) {
    QString synonyms;
    NwUtils::describe(reader, "synonyms", synonyms);
    synonyms = QUrl::fromPercentEncoding(synonyms.toLatin1());
    this->synonyms = synonyms.split(QRegExp("; "));
}

QString Entry::dateFormat = "yyyy-MM-dd";


///////////////////////////////////////////////////////////////////
//
// MAL Search result
//
//////////////////////////////////////////////////////////////////

SearchResult::SearchResult(CurlResult &result, QString query) :
    query(query)
{
    parse(result);
}

void SearchResult::parse(CurlResult &result) {
    std::cout.flush();
    nw::XmlReader xr(result.data);
    xr.push("anime");
    xr.describeArray("", "entry", 0);
    bool hasEntries = false;
    for (int i=0; xr.enterNextElement(i); ++i) {
        entries.append(Entry(xr));
        entries.back().calculateQuerySimiliarity(query);
        hasEntries = true;
    }
    if (!hasEntries) {
        qDebug() << "no results for mal search >" << query;
    }
    xr.close();
}

const Entry* SearchResult::bestResult() const {
    std::pair<int, const Entry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        const Entry* entry = &entries.at(i);

        int score = Utils::querySimiliarity(this->query, entry->title);
        /*
        foreach (const QString& name, entry->englishTitles) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        */
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        /*
        foreach (const QString& name, entry->japaneseTitles) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        */
        if (score > best.first) {
            best.first = score;
            best.second = entry;
        }
    }
    return best.second;
}

void SearchResult::updateShowFromBestEntry(TvShow &show, const Library& library) const {
    const Entry* entry = bestResult();

    if (entry) {
        entry->updateShow(show, library);
        qDebug() << "updated " << show.getShowType() << show.name();
    }
}


AnimeUpdateData::AnimeUpdateData(TvShow *show) {
    this->episode = show->episodeList().highestWatchedEpisodeNumber();
    this->status = calculateWatchStatus(show->getStatus());
    this->downloaded_episodes = show->episodeList().numberOfEpisodes();
    this->times_rewatched = std::max(0, show->getRewatchCount());
    this->rewatch_value = std::max(0, show->getRewatchMarker());

    score = -1;
    storage_type = -1; // int (will be updated to accomodate strings soon) // yeah sure soon...
    storage_value = -1; // wat
    times_rewatched = -1;
    rewatch_value = -1; // 0 - 10 ? dont know didn't check
    priority = -1; // 0 - 10 ? dont know didn't check
    enable_discussion = 0; // int. 1=enable, 0=disable
    enable_rewatching = -1; // int. 1=enable, 0=disable
    fansub_group = show->favouriteReleaseGroup();
    QStringList tags; // string. tags separated by commas
}

UpdateWatchStatus AnimeUpdateData::calculateWatchStatus(const TvShow::WatchStatus status) {
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

void AnimeUpdateData::describe(nw::Describer& de) {
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

QString AnimeUpdateData::toXml() {
    std::stringstream ss;
    nw::XmlWriter xw(ss);
    this->describe(xw);

    nw::Tag* tag = xw.getMotherTag();
    tag->setName("entry");
    tag->setCanBeAttributeRecursive(false);
    xw.close();
    return QString(ss.str().data());
}

TvShow::WatchStatus AnimeItemData::restoreStatus(int malStatusId) {
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

Thread* Client::getActiveThread() const
{
    return activeThread;
}

AnimeListData::AnimeListData() :
    error("noinit")
{
}

AnimeListData::AnimeListData(nw::Describer& de) {
    describe(de);
}

void AnimeListData::updateShows(QList<TvShow*> shows) {
    foreach (AnimeItemData item, items) {
        foreach (TvShow* show, shows) {
            if (show->getRemoteId() == item.series_animedb_id) {
                item.updateShow(show);
                break;
            }
        }
    }
}

const AnimeItemData* AnimeListData::getShow(const TvShow* show) const {
    int id = show->getRemoteId();
    foreach (const AnimeItemData& item, items) {
        if (item.series_animedb_id == id) {
            return &item;
        }
    }
    return NULL;
}

void AnimeListData::describe(nw::Describer& de) {
    error.clear();
    NwUtils::describe(de, "error", error);
    if (!error.isEmpty()) {
        return;
    }
    de.describeArray("", "anime", -1);
    for (int i=0; de.enterNextElement(i); ++i) {
        items.push_back(AnimeItemData(de));
    }
}

AnimeItemData::AnimeItemData(nw::Describer& de) {
    describe(de);
}

void AnimeItemData::describe(nw::Describer& de) {
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
    my_last_updated = QDateTime::fromTime_t(unixTimeUpdate);

    //QStringList my_tags; // separated by ", "
    my_status = AnimeItemData::restoreStatus(status);
}

void AnimeItemData::updateShow(TvShow* show) {
    if (!localIsUpToDate(show)) {
        qDebug() << this->my_last_updated << "newer" << show->getLastOnlineTrackerUpdate();
        show->episodeList().setMinimalWatched(this->my_watched_episodes);
        int marker = this->my_rewatching_ep == 0 ? -1 :this->my_rewatching_ep;
        int count = std::max(this->my_rewatching, show->getRewatchCount());
        show->setRewatchCount(count);
        show->setRewatchMarker(marker);
        show->setLastOnlineTrackerUpdate(this->my_last_updated);
    }
    if (show->getLastOnlineTrackerUpdate().isNull()) {
        show->setLastOnlineTrackerUpdate(this->my_last_updated);
    }
}

bool AnimeItemData::localIsUpToDate(const TvShow* show) const {
    if (show->getLastOnlineTrackerUpdate().isNull()) {
        return show->episodeList().highestWatchedEpisodeNumber(0) >= this->my_watched_episodes;
    }
    return show->getLastOnlineTrackerUpdate() >= this->my_last_updated;
}

bool AnimeItemData::remoteIsUpToDate(const TvShow* show) const {
    TvShow::WatchStatus status = show->getStatus();
    TvShow::WatchStatus statusMalWouldSendIfSynced = restoreStatus(AnimeUpdateData::calculateWatchStatus(status));
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

} // namespace
