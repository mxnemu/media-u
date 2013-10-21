#include "tvshow.h"
#include "nwutils.h"
#include "filedownloadthread.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"

TvShow::TvShow(QString name, QObject *parent) : QObject(parent) {
    this->mName = name;
    totalEpisodes = 0;
    remoteId = -1;
}

Season& TvShow::season(QString name) {
    for (QList<Season*>::iterator it = seasons.begin(); it != seasons.end(); ++it) {
        if ((*it)->name() == name) {
            return *(it.i->t());
        }
    }
    this->seasons.push_back(new Season(name, this));
    Season* s = this->seasons.back();
    connect(s, SIGNAL(watchCountChanged(int,int)), this, SLOT(watchedChanged(int,int)));
    return *s;
}

void TvShow::read(QDir &dir) {

    nw::JsonReader jr(dir.absoluteFilePath("tvShow.json").toStdString());
    jr.describeArray("seasons", "season", seasons.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        Season& s = season(QString());
        s.readAsElement(jr);
    }

    // TODO remove copy pasta code
    jr.describeValueArray("synonymes", synonyms.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        std::string s = synonyms.at(i).toStdString();
        jr.describeValue(s);
    }

    jr.push("playerSettings");
    this->playerSettings.describe(&jr);
    jr.pop();

    NwUtils::describe(jr, "remoteId", remoteId);
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
    jw.describeArray("seasons", "season", seasons.length());
    if (jw.hasState("detailed")) {
        for (int i=0; jw.enterNextElement(i); ++i) {
            Season& season = *seasons[i];
            season.writeDetailed(jw);
        }
    } else {
        for (int i=0; jw.enterNextElement(i); ++i) {
            Season& season = *seasons[i];
            season.writeAsElement(jw);
        }
    }

    jw.describeValueArray("synonymes", synonyms.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        std::string s = synonyms.at(i).toStdString();
        jw.describeValue(s);
    }

    jw.push("playerSettings");
    this->playerSettings.describe(&jw);
    jw.pop();

    NwUtils::describe(jw, "remoteId", remoteId);
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

void TvShow::importEpisode(MovieFile *episode) {
    Season& season = this->season(episode->seasonName());
    season.addEpisode(episode);
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
    return !airingStatus.isEmpty() && airingStatus.startsWith("Currently", Qt::CaseInsensitive);
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

QString TvShow::coverPath(QDir libaryPath) const {
    return this->directory(libaryPath).absoluteFilePath("cover");
}

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

QString TvShow::name() const {
    return mName;
}

int TvShow::episodesDownloaded() const {
    int sum = 0;
    for (QList<Season*>::const_iterator it = seasons.begin(); it != seasons.end(); ++it) {
        sum += (*it)->numberOfEpisodes();
    }
    return sum;
}

int TvShow::getWatchedEpisodes() const {
    int sum = 0;
    for (QList<Season*>::const_iterator it = seasons.begin(); it != seasons.end(); ++it) {
        sum += (*it)->numberOfWatchedEpisodes();
    }
    return sum;
}

int TvShow::highestWatchedEpisodeNumber() const {
    int highest = 0;
    for (QList<Season*>::const_iterator it = seasons.begin(); it != seasons.end(); ++it) {
        int num = (*it)->highestWatchedEpisodeNumber();
        highest = num > highest ? num : highest;
    }
    return highest;
}

QString TvShow::favouriteReleaseGroup() {
    for (QList<Season*>::const_iterator it = seasons.begin(); it != seasons.end(); ++it) {
        return (*it)->favouriteReleaseGroup(); // TOOD get rid of seasons
    }
    return QString();
}

MovieFile *TvShow::getEpisodeForPath(const QString& path) {
    for (int i=0; i < seasons.length(); ++i) {
        MovieFile* episode = seasons[i]->getEpisodeForPath(path);
        if (episode) {
            return episode;
        }
    }
    return NULL;
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

void TvShow::setRemoteId(const int &value)
{
    remoteId = value;
}


void TvShow::watchedChanged(int oldSeasonCount, int newSeasonCount) {
    int count = getWatchedEpisodes();
    emit watchCountChanged(count - newSeasonCount + oldSeasonCount, count);
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

QDateTime TvShow::lastWatchedDate() const {
    QDateTime latest;
    foreach(Season* s, seasons) {
        foreach(MovieFile* m, s->episodes) {
            if (m->getWatchedDate() > latest) {
                latest = m->getWatchedDate();
            }
        }
    }
    return latest;
}
