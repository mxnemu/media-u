#ifndef EPISODE_H
#define EPISODE_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>

#include "nwutils.h"
#include "moviefile.h"

class Episode : public QObject
{
    Q_OBJECT
public:
    explicit Episode(nw::Describer *jw, QObject *parent = 0);
    explicit Episode(QString path, QObject *parent = 0);
    explicit Episode(const MovieFile* path, QObject* parent = 0);
    
    void addPath(QString path);
    void addPath(const MovieFile* movieFile);

    void describe(nw::Describer *jw);
    void writeDetailed(nw::JsonWriter& jw);

    const MovieFile* getMovieFileForPath(QString path);
    const MovieFile* bestFile() const;

    QString getShowName() const;
    bool getWatched() const;
    void setWatched(bool value);

    int getEpisodeNumber() const;
    QDateTime getWatchedDate() const;

    bool isSpecial() const;
signals:
    void watchedChanged(bool oldValue, bool newValue);
    
public slots:
    
private:
    QList<const MovieFile*> files;
    QString showName;
    QDateTime watchedDate;
    int episodeNumber;

    void pushFile(const MovieFile*);
};

#endif // EPISODE_H
