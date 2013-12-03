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
    virtual ~Episode();
    
    void addPath(QString path);
    void addPath(const MovieFile* movieFile);

    void describe(nw::Describer *jw);
    void writeDetailed(nw::JsonWriter& jw);

    const MovieFile* getMovieFileForPath(QString path);
    const MovieFile* bestFile() const;

    QString getShowName() const;
    bool getWatched() const;
    void setWatched(bool value);

    float getEpisodeNumber() const;
    QDateTime getWatchedDate() const;

    bool isSpecial() const;
signals:
    void beforeWatchedChanged(bool newValue, bool oldValue);
    void watchedChanged(bool oldValue, bool newValue); // TODO change to old, new
    
public slots:
    
private:
    QList<const MovieFile*> files;
    QString showName;
    QDateTime watchedDate;
    float episodeNumber;

    void pushFile(const MovieFile*);
};

#endif // EPISODE_H
