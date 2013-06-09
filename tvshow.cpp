#include "tvshow.h"
#include "nwutils.h"
#include "filedownloadthread.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"

TvShow::TvShow(QString name) {
    this->mName = name;
    totalEpisodes = 0;
}

Season& TvShow::season(QString name) {
    for (QList<Season>::iterator it = seasons.begin(); it != seasons.end(); ++it) {
        if (it->name() == name) {
            return it.i->t();
        }
    }
    this->seasons.push_back(Season(name));
    return this->seasons.back();
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

void TvShow::write(QDir &dir) {

    nw::JsonWriter jw(dir.absoluteFilePath("tvShow.json").toStdString());
    jw.describeArray("seasons", "season", seasons.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        Season& season = seasons[i];
        season.writeAsElement(jw);
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
    jw.close();
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

        t->start(QThread::LowPriority);
    }
}

bool TvShow::isAiring() const {
    return !airingStatus.isEmpty() && airingStatus.startsWith("Currently");
}

QDir TvShow::directory(QDir libraryDirectory) const {
    return QDir(libraryDirectory.absoluteFilePath(this->name()));
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
    int randomIndex = rand() % walls.length();
    if (randomIndex < walls.length()) {
        return walls.at(randomIndex);
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

QString TvShow::name() const {
    return mName;
}

int TvShow::episodesDownloaded() const {
    int sum = 0;
    for (QList<Season>::const_iterator it = seasons.begin(); it != seasons.end(); ++it) {
        sum += it->numberOfEpisodes();
    }
    return sum;
}

int TvShow::getTotalEpisodes() const
{
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
