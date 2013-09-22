#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <qhttpconnection.h>
#include "tvshow.h"
#include "moviefile.h"
#include "malclient.h"
#include "moebooruclient.h"
#include "libraryfilter.h"
#include "searchdirectory.h"

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QString path, QObject *parent = 0);
    void initMalClient(QString malConfigFilepath);

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

    TvShow& tvShow(const QString name);
    TvShow* existingTvShow(const QString name);
    LibraryFilter& filter();
    void importTvShowEpisode(QString episodePath);

    void xbmcLinkExport(QDir outputDir);
    void write();
    void readAll();


    void fetchMetaData();
    QDir getDirectory() const;

    void downloadWallpapers();

    void startSearch();

    const QList<SearchDirectory>& getSearchDirectories() const;
    bool addSearchDirectory(SearchDirectory dir);
    SearchDirectory* getSearchDirectory(QString path);
    bool removeSearchDirectory(QString path);

signals:
    void showAdded(TvShow* show);
    
public slots:

private slots:
    void fetchingFinished();

private:
    QDir directory;
    QList<TvShow*> tvShows;
    QList<MovieFile*> movies;
    QList<SearchDirectory> searchDirectories;

    MalClient malClient;
    LibraryFilter mFilter;
    Moebooru::Client konachanClient;
    Moebooru::Client yandereClient;
};

#endif // LIBRARY_H
