#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <N0Slib.h>
#include <QDir>
#include "moviefile.h"

class EpisodeList : public QObject
{
    Q_OBJECT
public:
    EpisodeList(QString name = QString(), QObject *parent = NULL);
    virtual ~EpisodeList();

    void exportXbmcLinks(QDir dir);
    void writeDetailed(nw::JsonWriter &jw);
    void writeAsElement(nw::JsonWriter& jw);
    void readAsElement(nw::JsonReader &jr);

    void addEpisode(MovieFile *file); ///< this takes ownage
    void addEpisode(const MovieFile& file);
    void addEpisode(QString file);
    QString name() const;
    int numberOfEpisodes() const;
    int numberOfWatchedEpisodes() const;
    int highestWatchedEpisodeNumber() const;

    MovieFile* getEpisodeForPath(const QString &path);
    QString favouriteReleaseGroup() const;
    int highestDownloadedEpisodeNumber() const;

    // TODO
    // This class will be removed soon, so I just made this public for some last sloppy uses
    QList<MovieFile*> episodes;

signals:
    void watchCountChanged(int oldCount, int newCount);

private slots:
    void watchedChanged(bool oldValue, bool newValue);

private:

    QString mName;
};

#endif // SEASON_H
