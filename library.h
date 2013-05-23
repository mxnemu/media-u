#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "tvshow.h"
#include "moviefile.h"

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QString path, QObject *parent = 0);
    QString randomWallpaperPath() const;

    TvShow& tvShow(QString name);
    void importTvShowEpisode(QString episodePath);
signals:
    
public slots:

private:
    QDir directory;
    QList<TvShow> tvShows;
    QList<MovieFile> movies;
};

#endif // LIBRARY_H
