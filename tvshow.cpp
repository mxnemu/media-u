#include "tvshow.h"
#include "nwutils.h"
#include "filedownloadthread.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"

TvShow::TvShow(QString name, QObject *parent) : QObject(parent) {
    this->mName = name;
    totalEpisodes = 0;
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

    NwUtils::describe(jr, "remoteId", remoteId);
    jr.describe("totalEpisodes", totalEpisodes);
    NwUtils::describe(jr, "airingStatus", airingStatus);
    NwUtils::describe(jr, "startDate", startDate);
    NwUtils::describe(jr, "endDate", endDate);
    NwUtils::describe(jr, "synopsis", synopsis);
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

    NwUtils::describe(jw, "remoteId", remoteId);
    jw.describe("totalEpisodes", totalEpisodes);
    NwUtils::describe(jw, std::string("airingStatus"), airingStatus);
    NwUtils::describe(jw, "startDate", startDate);
    NwUtils::describe(jw, "endDate", endDate);
    NwUtils::describe(jw, "synopsis", synopsis);
}

void TvShow::importEpisode(const MovieFile &episode) {
    Season& season = this->season(episode.seasonName());
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
    return !airingStatus.isEmpty() && airingStatus.startsWith("Currently");
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

QString TvShow::getRemoteId() const
{
    return remoteId;
}

void TvShow::setRemoteId(const QString &value)
{
    remoteId = value;
}


void TvShow::watchedChanged(int oldSeasonCount, int newSeasonCount) {
    int count = getWatchedEpisodes();
    emit watchCountChanged(count - newSeasonCount + oldSeasonCount, count);
}
