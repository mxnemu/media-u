#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <QDate>
#include <QDir>
#include <season.h>
#include <N0Slib.h>

class TvShow
{
public:
    TvShow(QString name);

    Season &season(QString name);

    void read(QDir &dir);
    void write(nw::JsonWriter &jw);

    void importEpisode(const MovieFile& episode);
    void downloadImage(const QString url, QDir libraryDirectory);

    bool isAiring() const;
    QString coverPath(QDir libaryPath) const;

    QString name() const;
    QStringList getSynonyms() const;
    QString getAiringStatus() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    int getTotalEpisodes() const;
    QString getSynopsis() const;
    QString getShowType() const;
    QString getRemoteId() const;

    void setSynonyms(const QStringList &value);
    void setAiringStatus(const QString &value);
    void setStartDate(const QDate &value);
    void setEndDate(const QDate &value);
    void setTotalEpisodes(int value);
    void setShowType(const QString &value);
    void setSynopsis(const QString &value);
    void setRemoteId(const QString &value);

    QDir directory(QDir libraryDirectory) const;
    QDir wallpaperDirectory(QDir libraryDirectory) const;
    int numberOfWallpapers(QDir libraryDirectory) const;
    QString randomWallpaper(QDir libraryDirectory) const;
    QStringList wallpapers(QDir libraryDirectory) const;
    int episodesDownloaded() const;
private:

    QString mName;
    QList<Season> seasons;

    QString remoteId;
    QStringList synonyms;
    QString showType;
    QString airingStatus;
    QDate startDate;
    QDate endDate;
    int totalEpisodes;
    QString synopsis;
};

#endif // TVSHOW_H
