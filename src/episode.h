#ifndef EPISODE_H
#define EPISODE_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>

#include "nwutils.h"
#include "videofile.h"

class Episode : public QObject
{
    Q_OBJECT
public:
    explicit Episode(nw::Describer *jw, QObject *parent = 0);
    explicit Episode(QString path, QObject *parent = 0);
    explicit Episode(const VideoFile* path, QObject* parent = 0);
    virtual ~Episode();
    
    void addPath(QString path);
    void addPath(const VideoFile* movieFile);

    void describe(nw::Describer *jw);
    void writeDetailed(nw::JsonWriter& jw, const QStringList& releaseGroupPreference);

    const VideoFile* getMovieFileForPath(QString path);
    const VideoFile* bestFile(const QStringList& releaseGroupPreference) const;

    QString getShowName() const;
    bool getWatched() const;
    void setWatched(bool value);

    float getEpisodeNumber() const;
    QDateTime getWatchedDate() const;
    QStringList releaseGroups() const;

    bool isSpecial() const;
    QList<const VideoFile*> missingFiles() const;
signals:
    void beforeWatchedChanged(bool newValue, bool oldValue);
    void watchedChanged(bool oldValue, bool newValue); // TODO change to old, new
    
public slots:
    
private:
    QList<const VideoFile*> files;
    QString showName;
    QDateTime watchedDate;
    float episodeNumber;

    void pushFile(const VideoFile*);
};

#endif // EPISODE_H
