#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <N0Slib.h>
#include <QDir>
#include "episode.h"

class EpisodeList : public QObject
{
    Q_OBJECT
public:
    EpisodeList(QObject *parent = NULL);
    virtual ~EpisodeList();

    void exportXbmcLinks(QDir dir);
    void writeDetailed(nw::JsonWriter &jw, const QStringList& releaseGroupPreference);
    void writeAsElement(nw::JsonWriter& jw);
    void readAsElement(nw::JsonReader &jr);

    void addMovieFile(const MovieFile *movieFile); ///< this takes ownage
    void addEpisode(Episode* episode); ///< this takes ownage

    Episode* getEpisodeForNumber(float number);

    int numberOfEpisodes() const;
    int numberOfWatchedEpisodes() const;
    float highestWatchedEpisodeNumber() const;

    Episode* getEpisodeForPath(const QString &path);
    QString mostDownloadedReleaseGroup() const;
    QString favouriteReleaseGroup() const;
    float highestDownloadedEpisodeNumber() const;
    QStringList releaseGroups() const;

    // TODO
    // move stuff that uses this into this class
    QList<Episode*> episodes;
    void setMinimalWatched(int number);
    void setWatched(int number);
signals:
    void beforeWatchCountChanged(int newCount, int oldCount);
    void watchCountChanged(int oldCount, int newCount); // TODO new, old

private slots:
    void beforeWatchedChanged(bool newValue, bool oldValue);
    void watchedChanged(bool oldValue, bool newValue); // TODO new, old

private:
};

#endif // SEASON_H
