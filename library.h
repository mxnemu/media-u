#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "tvshow.h"
#include "moviefile.h"
#include "malclient.h"

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QString path, QObject *parent = 0);
    void initMalClient(QString malConfigFilepath);

    QString randomWallpaperPath() const;

    TvShow& tvShow(QString name);
    void importTvShowEpisode(QString episodePath);


    QList<const TvShow *> airingShows() const;

    void write();
    void readAll();

    void fetchMetaData();
signals:
    
public slots:

private slots:
    void fetchingFinished();

private:
    QDir directory;
    QList<TvShow> tvShows;
    QList<MovieFile> movies;
    MalClient malClient;
};

#endif // LIBRARY_H
