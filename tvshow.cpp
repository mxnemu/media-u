#include "tvshow.h"
#include "nwutils.h"
#include "filedownloadthread.h"

TvShow::TvShow(QString name) {
    this->mName = name;
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

void TvShow::write(QDir &dir) {
    nw::JsonWriter jw(dir.absoluteFilePath("tvShow.json").toStdString());
    jw.describeArray("seasons", "season", seasons.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        const Season& season = seasons.at(i);
        season.writeAsElement(jw);
    }

    jw.describeValueArray("synonymes", synonyms.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        std::string s = synonyms.at(i).toStdString();
        jw.describeValue(s);
    }

    NwUtils::describe(jw, std::string("airingStatus"), airingStatus);
    NwUtils::describe(jw, "airingStatus", startDate);
    NwUtils::describe(jw, "airingStatus", endDate);
    jw.describe("totalEpisodes", totalEpisodes);

    jw.close();
}

void TvShow::importEpisode(const MovieFile &episode) {
    Season& season = this->season(episode.seasonName());
    season.addEpisode(episode);
}

void TvShow::downloadImage(const QString url) {
    if (!url.isEmpty() && !url.isNull()) {
        //FileDownloadThread* t = new FileDownloadThread(url, );
        // TODO implement
    }
}

QString TvShow::name() const {
    return mName;
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
