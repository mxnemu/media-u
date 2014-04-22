#include "tvshow.h"
#include "nwutils.h"
#include "filedownloadthread.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"
#include "server.h"

TvShow::TvShow(QString name, QObject *parent) : QObject(parent) {
    this->mName = name;
    totalEpisodes = 0;
    remoteId = -1;
    customStatus = automatic;
    rewatchMarker = -1;
    rewatchCount = 0;
    connect(&this->episodes, SIGNAL(watchCountChanged(int,int)), this, SLOT(onWatchCountChanged(int, int)));
}

EpisodeList& TvShow::episodeList() {
    return episodes;
}

void TvShow::read(QDir &dir) {

    nw::JsonReader jr(dir.absoluteFilePath("tvShow.json").toStdString());
    episodes.readAsElement(jr);

    NwUtils::describeValueArray(jr, "synonymes", synonyms);

    jr.push("playerSettings");
    this->playerSettings.describe(&jr);
    jr.pop();
    NwUtils::describeValueArray(jr, "releaseGroupPreference", releaseGroupPreference);

    QString customStatusString;
    NwUtils::describe(jr, "customStatus", customStatusString);
    this->customStatus = watchStatusFromString(customStatusString);
    NwUtils::describe(jr, "rewatchMarker", rewatchMarker);
    NwUtils::describe(jr, "rewatchCount", rewatchCount);


    NwUtils::describe(jr, "remoteId", remoteId);
    NwUtils::describe(jr, "lastOnlineTrackerUpdate", lastOnlineTrackerUpdate);
    NwUtils::describe(jr, "lastLocalUpdate", lastLocalUpdate);
    jr.describe("totalEpisodes", totalEpisodes);
    NwUtils::describe(jr, "airingStatus", airingStatus);
    NwUtils::describe(jr, "startDate", startDate);
    NwUtils::describe(jr, "endDate", endDate);
    NwUtils::describe(jr, "synopsis", synopsis);


    prequels.clear();
    jr.describeArray("prequels", "tvShowRelation", prequels.length());
    for (int i=0; i < jr.enterNextElement(i); ++i) {
        prequels.push_back(RelatedTvShow());
        prequels[i].describe(&jr);
    }
    sideStories.clear();
    jr.describeArray("sideStories", "tvShowRelation", sideStories.length());
    for (int i=0; i < jr.enterNextElement(i); ++i) {
        sideStories.push_back(RelatedTvShow());
        sideStories[i].describe(&jr);
    }
    sequels.clear();
    jr.describeArray("sequels", "tvShowRelation", sequels.length());
    for (int i=0; i < jr.enterNextElement(i); ++i) {
        sequels.push_back(RelatedTvShow());
        sequels[i].describe(&jr);
    }

    jr.close();
}

void TvShow::write(nw::JsonWriter& jw) {
    NwUtils::describe(jw, "name", mName);
    if (jw.hasState("detailed")) {
        QStringList groups = getReleaseGroupPreference();
        episodes.writeDetailed(jw, groups);
        //QStringList oldGroups = releaseGroupPreference;
        NwUtils::describeValueArray(jw, "releaseGroupPreference", groups);
        //releaseGroupPreference = oldGroups;
    } else {
        episodes.writeAsElement(jw);
        NwUtils::describeValueArray(jw, "releaseGroupPreference", releaseGroupPreference);
    }

    jw.describeValueArray("synonymes", synonyms.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        std::string s = synonyms.at(i).toStdString();
        jw.describeValue(s);
    }

    jw.push("playerSettings");
    this->playerSettings.describe(&jw);
    jw.pop();

    QString customStatusString = watchStatusToString(customStatus);
    NwUtils::describe(jw, "customStatus", customStatusString);
    NwUtils::describe(jw, "rewatchMarker", rewatchMarker);
    NwUtils::describe(jw, "rewatchCount", rewatchCount);

    NwUtils::describe(jw, "remoteId", remoteId);
    NwUtils::describe(jw, "lastOnlineTrackerUpdate", lastOnlineTrackerUpdate);
    NwUtils::describe(jw, "lastLocalUpdate", lastLocalUpdate);
    jw.describe("totalEpisodes", totalEpisodes);
    NwUtils::describe(jw, std::string("airingStatus"), airingStatus);
    NwUtils::describe(jw, "startDate", startDate);
    NwUtils::describe(jw, "endDate", endDate);
    NwUtils::describe(jw, "synopsis", synopsis);

    jw.describeArray("prequels", "tvShowRelation", prequels.length());
    for (int i=0; i < jw.enterNextElement(i); ++i) {
        prequels[i].describe(&jw);
    }
    jw.describeArray("sideStories", "tvShowRelation", sideStories.length());
    for (int i=0; i < jw.enterNextElement(i); ++i) {
        sideStories[i].describe(&jw);
    }
    jw.describeArray("sequels", "tvShowRelation", sequels.length());
    for (int i=0; i < jw.enterNextElement(i); ++i) {
        sequels[i].describe(&jw);
    }
    jw.close();
}

void TvShow::writeAsListingItem(nw::Describer* de) const {
    int watched = this->episodes.numberOfWatchedEpisodes();
    int downloaded = this->episodes.numberOfEpisodes();
    int total = this->totalEpisodes;
    QString statusStr = watchStatusToString(this->getStatus());
    QString nameStr = this->name();

    NwUtils::describe(*de, "name", nameStr);
    de->describe("watchedEpisodes", watched);
    de->describe("downloadedEpisodes", downloaded);
    de->describe("totalEpisodes", total);
    NwUtils::describe(*de, "status", statusStr);
}



void TvShow::importVideoFile(const VideoFile *episode) {
    episodes.addMovieFile(episode);
}

void TvShow::downloadImage(const QString url, QDir libraryDirectory) {
    if (!url.isEmpty() && !url.isNull()) {
        FileDownloadThread* t = new FileDownloadThread(url, directory(libraryDirectory).absoluteFilePath("cover"));
        //connect(t, SIGNAL(finished()),
        //        this, SLOT(posterDownloadFinished()));
        QObject::connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        t->start(QThread::LowPriority);
    }
}

bool TvShow::isAiring() const {
    // WORKAROUND call things did not air yet 'airing'
    // this works okay for all current scenarios, but doesn't fit the function name
    // TODO rename function and create another fn with a good name that checks for both states
    return !airingStatus.isEmpty() && (
                airingStatus.startsWith("Currently", Qt::CaseInsensitive) ||
                airingStatus.startsWith("Not yet", Qt::CaseInsensitive)
            );
}

QDir TvShow::directory(QDir libraryDirectory) const {
    return QDir(libraryDirectory.absoluteFilePath(this->name().toLower()));
}

QDir TvShow::wallpaperDirectory(QDir libraryDirectory) const {
    return directory(libraryDirectory).absoluteFilePath("wallpapers");
}

int TvShow::numberOfWallpapers(QDir libraryDirectory) const {
    return wallpapers(libraryDirectory).length();
}

QString TvShow::randomWallpaper(QDir libraryDirectory) const
{
    QStringList walls = wallpapers(libraryDirectory);
    if (!walls.empty()) {
        int randomIndex = rand() % walls.length();
        if (randomIndex < walls.length()) {
            return walls.at(randomIndex);
        }
    }
    return QString();
}

QStringList TvShow::wallpapers(QDir libraryDirectory) const {
    DirectoryScanner ds;
    FileFilterScanner* ffs = new FileFilterScanner(QRegExp(".*"));
    ds.addScanner(ffs); // will delete
    ds.scan(wallpaperDirectory(libraryDirectory).absolutePath());
    return ffs->getMatchedFiles();
}

QString TvShow::favouriteReleaseGroup() {
    if (releaseGroupPreference.isEmpty()) {
        return this->episodeList().mostDownloadedReleaseGroup();
    }
    return releaseGroupPreference.front();
}

QString TvShow::coverPath(QDir libaryPath) const {
    return this->directory(libaryPath).absoluteFilePath("cover");
}

void TvShow::handleApiRequest(int urlPrefixOffset, QHttpRequest* req, QHttpResponse* resp) {
    if (urlPrefixOffset == req->path().indexOf("/details", urlPrefixOffset)) {
        std::stringstream ss;
        nw::JsonWriter jw(ss);
        jw.setState("detailed", true);
        this->write(jw);
        jw.close();
        Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
    } else if (urlPrefixOffset == req->path().indexOf("/setRewatchMarker", urlPrefixOffset)) {
        bool ok = false;
        int marker = req->url().query(QUrl::FullyDecoded).toInt(&ok);
        if (ok) {
            this->setRewatchMarker(marker, true);
            Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
            return;
        }
        Server::simpleWrite(resp, 400, "{\"error\":\"invalid number\"}", mime::json);
    } else if (urlPrefixOffset == req->path().indexOf("/playerSettings", urlPrefixOffset)) {
        if (req->method() == QHttpRequest::HTTP_PUT) {
            RequestBodyListener* bodyListener = new RequestBodyListener(resp, this);
            connect(req, SIGNAL(data(QByteArray)), bodyListener, SLOT(onDataReceived(QByteArray)));
            connect(bodyListener, SIGNAL(bodyReceived(QHttpResponse*,const QByteArray&)), this, SLOT(receivedPlayerSettings(QHttpResponse*,const QByteArray&)));
        } else if (req->method() == QHttpRequest::HTTP_GET) {
            std::stringstream ss;
            nw::JsonWriter jw(ss);
            this->playerSettings.describe(&jw);
            jw.close();
            Server::simpleWrite(resp, 200, ss.str().data(), mime::json);
        } else {
            Server::simpleWrite(resp, 400, "page does not have a show set");
        }
    } else if (urlPrefixOffset == req->path().indexOf("/releaseGroupPreference")) {
        if (req->method() == QHttpRequest::HTTP_PUT) {
            RequestBodyListener* bodyListener = new RequestBodyListener(resp, this);
            connect(req, SIGNAL(data(QByteArray)), bodyListener, SLOT(onDataReceived(QByteArray)));
            connect(bodyListener, SIGNAL(bodyReceived(QHttpResponse*,const QByteArray&)), this, SLOT(receivedReleaseGroupPreference(QHttpResponse*,const QByteArray&)));
        } else {
            Server::simpleWrite(resp, 400, "page does not have a show set");
        }
    } else if (urlPrefixOffset == req->path().indexOf("/setStatus")) {
        TvShow::WatchStatus status = TvShow::watchStatusFromString(req->url().query(QUrl::FullyDecoded));
        this->setStatus(status);
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else {
        Server::simpleWrite(resp, 400, "{\"error\":\"no api for url path\"}", mime::json);
    }
}


void TvShow::receivedPlayerSettings(QHttpResponse *resp, const QByteArray& body) {
    std::stringstream ss;
    ss << body.data();
    nw::JsonReader jr(ss);

    if (jr.getErrorMessage().empty()) {
        this->playerSettings.describe(&jr);
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else {
        Server::simpleWrite(resp, 400, "{\"error\": \"no valid json provided in query\"}", mime::json);
    }
    jr.close();
}

void TvShow::receivedReleaseGroupPreference(QHttpResponse* resp, const QByteArray& body) {
    std::stringstream ss;
    ss << body.data();
    nw::JsonReader jr(ss);

    if (jr.getErrorMessage().empty()) {
        QStringList groups;
        NwUtils::describeValueArray(jr, "releaseGroupPreference", groups);
        this->setReleaseGroupPreference(groups);
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else {
        Server::simpleWrite(resp, 400, "{\"error\": \"no valid json array provided in query\"}", mime::json);
    }
    jr.close();
}

void TvShow::onWatchCountChanged(int, int) {
    this->lastLocalUpdate = QDateTime::currentDateTimeUtc();
}

/*
void TvShow::exportXbmcLinks(QDir dir) {
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    int i = 0;
    for (QList<Season*>::iterator it = seasons.begin(); it != seasons.end(); ++it) {
        QString seasonName = (*it)->name().length() > 0 ? (*it)->name() :
                             QString("Season %1").arg(i+1);
        QDir seasonDir(dir.absoluteFilePath(seasonName));
        (*it)->exportXbmcLinks(seasonDir);
        ++i;
    }
}
*/

QString TvShow::name() const {
    return mName;
}

int TvShow::getTotalEpisodes() const {
    return totalEpisodes;
}

void TvShow::setTotalEpisodes(int value)
{
    totalEpisodes = value;
}

QDate TvShow::getEndDate() const
{
    return endDate;
}

void TvShow::setEndDate(const QDate &value)
{
    endDate = value;
}

QDate TvShow::getStartDate() const
{
    return startDate;
}

void TvShow::setStartDate(const QDate &value)
{
    startDate = value;
}

QString TvShow::getAiringStatus() const
{
    return airingStatus;
}

void TvShow::setAiringStatus(const QString &value)
{
    airingStatus = value;
}

QStringList TvShow::getSynonyms() const
{
    return synonyms;
}

void TvShow::setSynonyms(const QStringList &value)
{
    synonyms = value;
}

QString TvShow::getSynopsis() const
{
    return synopsis;
}

void TvShow::setSynopsis(const QString &value)
{
    synopsis = value;
}

QString TvShow::getShowType() const
{
    return showType;
}

void TvShow::setShowType(const QString &value)
{
    showType = value;
}

int TvShow::getRemoteId() const
{
    return remoteId;
}

bool TvShow::matchesNameOrSynonym(QString str) const {
    return 0 == this->mName.compare(str, Qt::CaseInsensitive) ||
            this->synonyms.contains(str, Qt::CaseInsensitive);
}

void TvShow::setRemoteId(const int &value)
{
    remoteId = value;
}

TvShowPlayerSettings::TvShowPlayerSettings() :
    subtileTrack(0),
    audioTrack(0),
    videoTrack(0)
{
}

void TvShowPlayerSettings::describe(nw::Describer*de) {
    NwUtils::describe(*de, "subtitleTrack", subtileTrack);
    NwUtils::describe(*de, "audioTrack", audioTrack);
    NwUtils::describe(*de, "videoTrack", videoTrack);
}


RelatedTvShow::RelatedTvShow(int id) :
    id(id)
{
}

TvShow *RelatedTvShow::get(Library& library) const {
    return library.filter().getShowForRemoteId(this->id);
}

bool RelatedTvShow::operator ==(const RelatedTvShow &other) const {
    return this->id == other.id; //|| this->title.compare(other.title, Qt::CaseInsensitive) == 0;
}

void RelatedTvShow::describe(nw::Describer *de) {
    NwUtils::describe(*de, "id", id);
    NwUtils::describe(*de, "title", title);
}

void RelatedTvShow::parseForManga(nw::Describer* de) {
    NwUtils::describe(*de, "manga_id", id);
    NwUtils::describe(*de, "title", title);
}

void RelatedTvShow::parseForAnime(nw::Describer* de) {
    NwUtils::describe(*de, "anime_id", id);
    NwUtils::describe(*de, "title", title);
}

void RelatedTvShow::parseFromList(nw::Describer *de, QString arrayName, QList<RelatedTvShow>& list, const bool anime) {
    list.empty();
    de->describeArray(arrayName.toUtf8().data(), "", 0);
    for (int i=0; de->enterNextElement(i); ++i) {
        RelatedTvShow entry;
        if (anime) {
            entry.parseForAnime(de);
        } else {
            entry.parseForManga(de);
        }
        list.append(entry);
    }
}

void TvShow::addPrequels(QList<RelatedTvShow> relations) {
    foreach (const RelatedTvShow& rel, relations) {
        if (!this->prequels.contains(rel) && rel.id != -1) {
            this->prequels.append(rel);
        }
    }
}

void TvShow::addSideStories(QList<RelatedTvShow> relations) {
    foreach (const RelatedTvShow& rel, relations) {
        if (!this->sideStories.contains(rel) && rel.id != -1) {
            this->sideStories.append(rel);
        }
    }
}

void TvShow::addSequels(QList<RelatedTvShow> relations) {
    foreach (const RelatedTvShow& rel, relations) {
        if (!this->sequels.contains(rel) && rel.id != -1) {
            this->sequels.append(rel);
        }
    }
}

void TvShow::syncRelations(Library& library) {
    if (this->remoteId == -1) {
        return;
    }

    QList<RelatedTvShow> relation;
    relation.append(RelatedTvShow(this->remoteId));
    foreach (const RelatedTvShow& rel, prequels) {
        TvShow* show = rel.get(library);
        if (show) {
            show->addSequels(relation);
        }
    }
    foreach (const RelatedTvShow& rel, sideStories) {
        TvShow* show = rel.get(library);
        if (show) {
            show->addSideStories(relation);
        }
    }
    foreach (const RelatedTvShow& rel, sequels) {
        TvShow* show = rel.get(library);
        if (show) {
            show->addPrequels(relation);
        }
    }
}

bool TvShow::hasRelationTo(const TvShow *show) const {
    if (show->remoteId == -1) {
        return false;
    }

    foreach (const RelatedTvShow& rel, prequels) {
        if (rel.id == show->remoteId) {
            return true;
        }
    }
    foreach (const RelatedTvShow& rel, sideStories) {
        if (rel.id == show->remoteId) {
            return true;
        }
    }
    foreach (const RelatedTvShow& rel, sequels) {
        if (rel.id == show->remoteId) {
            return true;
        }
    }
    return false;
}

void TvShow::addSynonyms(const QStringList &values) {
    foreach (const QString& value, values) {
        bool found = false;
        foreach (const QString& synonym, synonyms) {
            if (synonym.compare(value, Qt::CaseInsensitive) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            synonyms.push_back(value);
        }
    }
}

// TODO move to episodelist
QDateTime TvShow::lastWatchedDate() const {
    QDateTime latest;
    foreach(Episode* m, episodes.episodes) {
        if (m->getWatchedDate() > latest) {
            latest = m->getWatchedDate();
        }
    }
    return latest;
}

QString TvShow::watchStatusToString(TvShow::WatchStatus status) {
    if (status == automatic) return "automatic";
    if (status == completed) return "completed";
    if (status == watching) return "watching";
    if (status == waitingForNewEpisodes) return "waitingForNewEpisodes";
    if (status == onHold) return "onHold";
    if (status == dropped) return "dropped";
    if (status == planToWatch) return "planToWatch";
    return "";
}

TvShow::WatchStatus TvShow::watchStatusFromString(QString status) {
    if (status == "automatic") return automatic;
    if (status == "completed") return completed;
    if (status == "watching") return watching;
    if (status == "waitingForNewEpisodes") return waitingForNewEpisodes;
    if (status == "onHold") return onHold;
    if (status == "dropped") return dropped;
    if (status == "planToWatch") return planToWatch;
    return automatic;
}

TvShow::WatchStatus TvShow::getStatus() const {
    if (customStatus != automatic) {
        return customStatus;
    }

    int eps = episodes.numberOfEpisodes();
    int total = std::max(totalEpisodes, episodes.numberOfEpisodes());
    total = std::max(total, eps);
    bool unknownAiring = totalEpisodes <= 0 && isAiring();
    if (unknownAiring || episodes.numberOfWatchedEpisodes() < total) {
        if (episodes.numberOfWatchedEpisodes() > 0) {
            if (lastWatchedDate() > QDateTime::currentDateTime().addMonths(-1)) {

                if ((unknownAiring || total > eps) && eps == episodes.numberOfWatchedEpisodes()) {

                    return waitingForNewEpisodes;
                }
                return watching;
            }
            return onHold;
        }
        return planToWatch;
    }
    return completed;
}

void TvShow::setStatus(TvShow::WatchStatus status) {
    if (customStatus != status) {
        if (status == completed) {
            this->episodeList().setWatched(this->episodeList().highestDownloadedEpisodeNumber());
        }
        if (status == planToWatch) {
            this->episodeList().setWatched(0);
        }
    }
    this->customStatus = status;
}

bool TvShow::isCompleted() const {
    int total = std::max(totalEpisodes, episodes.numberOfEpisodes());
    return !isAiring() && episodes.numberOfWatchedEpisodes() >= total;
}

const EpisodeList &TvShow::episodeList() const {
    return episodes;
}

QStringList TvShow::getReleaseGroupPreference() const {
    QStringList copy = releaseGroupPreference;
    QStringList allGroups = episodeList().releaseGroups();
    foreach (QString group, allGroups) {
        if (!copy.contains(group)) {
            copy.push_front(group);
        }
    }
    return copy;
}

void TvShow::setReleaseGroupPreference(QStringList value) {
    releaseGroupPreference = value;
}

int TvShow::getRewatchMarker() const {
    return this->rewatchMarker;
}

void TvShow::setRewatchMarker(int marker, bool updateTracker) {
    this->rewatchMarker = marker;
    if (marker == std::max((int)this->episodeList().highestDownloadedEpisodeNumber(), this->totalEpisodes)) {
        ++rewatchCount;
    }
    if (updateTracker) {
        this->lastLocalUpdate = QDateTime::currentDateTimeUtc();
    }
}

int TvShow::getRewatchCount() const {
    return rewatchCount;
}

void TvShow::setRewatchCount(int count, bool updateTracker) {
    this->rewatchCount = count;
    if (updateTracker) {
        this->lastLocalUpdate = QDateTime::currentDateTimeUtc();
    }
}

QDateTime TvShow::getLastLocalUpdate() const {
    return lastLocalUpdate;
}


QDateTime TvShow::getLastOnlineTrackerUpdate() const {
    return lastOnlineTrackerUpdate;
}

void TvShow::setLastOnlineTrackerUpdate(const QDateTime& value) {
    lastOnlineTrackerUpdate = value;
    lastLocalUpdate = value;
}
