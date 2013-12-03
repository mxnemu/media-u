#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <QDate>
#include <QDir>
#include "episodelist.h"
#include <N0Slib.h>

class TvShowPlayerSettings {
public:
    TvShowPlayerSettings();
    int subtileTrack;
    int audioTrack;
    int videoTrack;
    void describe(nw::Describer *de);
};

class TvShow;
class Library;
class RelatedTvShow {
public:
    RelatedTvShow(int id = -1);
    int id;
    QString title;
    TvShow* get(Library &library) const;

    bool operator ==(const RelatedTvShow& other) const;
    void describe(nw::Describer* de);
    void parseForManga(nw::Describer* de);
    void parseForAnime(nw::Describer* de);
    static void parseFromList(nw::Describer* de, QString arrayName, QList<RelatedTvShow> &list, const bool anime);
};

class TvShow : public QObject
{
    Q_OBJECT
public:

    enum WatchStatus {
        completed,
        watching,
        waitingForNewEpisodes,
        onHold,
        dropped,
        planToWatch
    };

    TvShow(QString name, QObject* parent = NULL);

    EpisodeList &episodeList();

    void read(QDir &dir);
    void write(nw::JsonWriter &jw);

    void importMovieFile(const MovieFile *episode);
    void downloadImage(const QString url, QDir libraryDirectory);

    bool isAiring() const;
    QString coverPath(QDir libaryPath) const;

    // TODO reimplement for episode list
    // or remove, since it didn't work very good to begin with
    //void exportXbmcLinks(QDir dir);

    QString name() const;
    QStringList getSynonyms() const;
    QString getAiringStatus() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    int getTotalEpisodes() const;
    QString getSynopsis() const;
    QString getShowType() const;
    int getRemoteId() const;

    void addPrequels(QList<RelatedTvShow> relations);
    void addSideStories(QList<RelatedTvShow> relations);
    void addSequels(QList<RelatedTvShow> relations);
    void syncRelations(Library &library);
    bool hasRelationTo(const TvShow *show) const;

    void addSynonyms(const QStringList& values);
    void setSynonyms(const QStringList &value);
    void setAiringStatus(const QString &value);
    void setStartDate(const QDate &value);
    void setEndDate(const QDate &value);
    void setTotalEpisodes(int value);
    void setShowType(const QString &value);
    void setSynopsis(const QString &value);
    void setRemoteId(const int &value);

    QDir directory(QDir libraryDirectory) const;
    QDir wallpaperDirectory(QDir libraryDirectory) const;
    int numberOfWallpapers(QDir libraryDirectory) const;
    QString randomWallpaper(QDir libraryDirectory) const;
    QStringList wallpapers(QDir libraryDirectory) const;
    QString favouriteReleaseGroup();

    Episode* getEpisodeForPath(const QString &path);

    TvShowPlayerSettings playerSettings;
    QDateTime lastWatchedDate() const;

    WatchStatus getStatus() const;
    bool isCompleted() const;
    bool startedWatching() const;
    const EpisodeList& episodeList() const;
    EpisodeList &episodeListMutable();
    static QString watchStatusToString(TvShow::WatchStatus status);
    void writeAsListingItem(nw::Describer *de) const;
signals:

private:

    QString mName;
    EpisodeList episodes;

    QList<RelatedTvShow> prequels;
    QList<RelatedTvShow> sideStories;
    QList<RelatedTvShow> sequels;

    int remoteId;
    QStringList synonyms;
    QString showType;
    QString airingStatus;
    QDate startDate;
    QDate endDate;
    int totalEpisodes;
    QString synopsis;
};

#endif // TVSHOW_H
