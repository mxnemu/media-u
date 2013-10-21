#include "library.h"
#include "nwutils.h"
#include <QDebug>
#include "server.h"
#include "directoryscanner.h"
#include "tvshowscanner.h"
#include <QStandardPaths>

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path),
    mFilter(tvShows, directory),
    searchThread(NULL)
{
    if (!directory.exists() && !QDir::root().mkpath(directory.absolutePath())) {
        qDebug() << "could not create library dir";
    }
    wallpaperDownloaders.push_back(new Moebooru::Client(("http://konachan.com")));
    wallpaperDownloaders.push_back(new Moebooru::Client(("https://yande.re")));
    wallpaperDownloaders.push_back(new Gelbooru::Client());

    for (int i=0; i < wallpaperDownloaders.length(); ++i) {
        wallpaperDownloaders[i]->setParent(this);
        connect(wallpaperDownloaders[i], SIGNAL(wallpaperDownloaded(QString)), this, SIGNAL(wallpaperDownloaded(QString)));
    }

    connect(&malapiClient, SIGNAL(updateFinished()),
            this, SLOT(fetchingFinished()));
}

void Library::initMalClient(QString malConfigFilepath) {
    malClient.init(malConfigFilepath);
    connect(&malClient, SIGNAL(fetchingFinished()),
            this, SLOT(fetchingFinished()));
}

bool Library::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path().startsWith("/api/library/filter")) {
        return filter().handleApiRequest(req, resp);
    } else if(req->path().startsWith("/api/library/randomWallpaper")) {
        // TODO change abs img path to server url
        Server::simpleWrite(resp, 200, QString("{\"image\":\"%1\"}").arg(filter().getRandomWallpaper()), mime::json);
    } else if(req->path().startsWith("/api/library/toggleWatched")) {
        MovieFile* episode = filter().getEpisodeForPath(req->url().query(QUrl::FullyDecoded));
        if (episode) {
            episode->setWatched(!episode->getWatched());
            Server::simpleWrite(resp, 200, QString("{\"watched\":%1}").arg(episode->getWatched() ? "true" : "false"), mime::json);
        } else {
            Server::simpleWrite(resp, 400, QString("{\"error\":\"Episode not found\"}"), mime::json);
        }
    } else {
        return false;
    }
    return true;
}

TvShow& Library::tvShow(const QString name) {
    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if ((*it)->name().compare(name, Qt::CaseInsensitive) == 0) {
            return *(it.i->t());
        }
    }
    this->tvShows.push_back(new TvShow(name, this));
    TvShow* show = this->tvShows.back();
    emit showAdded(show);
    return *show;
}

TvShow* Library::existingTvShow(const QString name) {
    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if ((*it)->name() == name) {
            return it.i->t();
        }
    }
    return NULL;
}

LibraryFilter &Library::filter()
{
    return mFilter;
}

void Library::importTvShowEpisode(QString episodePath) {
    MovieFile* episode = new MovieFile(episodePath);
    if (!episode->showName().isEmpty()) {
        TvShow& show = this->tvShow(episode->showName());
        show.importEpisode(episode); // takes ownage
    } else {
        qDebug() << "could not import (no show name parsed):" << episode->path();
        delete episode;
    }
}

void Library::xbmcLinkExport(QDir outputDir) {
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        (*it)->exportXbmcLinks((*it)->directory(outputDir));
    }
}

void Library::fetchMetaData() {
    malClient.fetchShows(tvShows, directory);
    //malapiClient.startUpdate(tvShows, directory);
}

void Library::startWallpaperDownloaders() {
    if (getWallpaperDownloadRunning()) {
        return;
    }
    for (int i=0; i < wallpaperDownloaders.size(); ++i) {
        WallpaperDownload::Client* downloader = wallpaperDownloaders[i];
        WallpaperDownload::FetchThread* ft = new WallpaperDownload::FetchThread(*downloader, filter().all(), directory, this);
        //connect(this, SIGNAL(destroyed()), ft, SLOT(terminate()));
        connect(ft, SIGNAL(finished()), ft, SLOT(deleteLater()));
        connect(ft, SIGNAL(finished()), this, SLOT(wallpaperDownloaderFinished()));
        runningWallpaperDownloaders.push_back(ft);
        ft->start(QThread::LowPriority);
    }
}

void Library::startSearch() {
    if (this->searchThread) {
        if (this->searchThread->isFinished()) {
            QThread* t = this->searchThread;
            this->searchThread = NULL;
            delete t;
        } else {
            return;
        }
    }
    DirectoryScanner* scanner = new DirectoryScanner();
    scanner->addScanner(new TvShowScanner(*this));
    this->searchThread = new DirectoryScannerThread(scanner, searchDirectories, this);
    //connect(this, SIGNAL(destroyed()), searchThread, SLOT(terminate()));
    connect(searchThread, SIGNAL(done()), this, SIGNAL(searchFinished()));
    connect(searchThread, SIGNAL(machingFile(QString)), this, SLOT(importTvShowEpisode(QString)));
    connect(searchThread, SIGNAL(done()), this, SLOT(startWallpaperDownloaders()));
    connect(searchThread, SIGNAL(done()), this, SLOT(fetchMetaData()));
    connect(searchThread, SIGNAL(done()), this, SLOT(generateFrenchises()));
    this->searchThread->start(QThread::HighPriority);
    //library.write();
}

Library::searchStatus Library::getSearchStatus() {
    if (this->searchThread) {
        return this->searchThread->isRunning() ? inProgress : done;
    }
    return notStarted;
}

bool Library::getWallpaperDownloadRunning() {
    return !runningWallpaperDownloaders.empty();
}

const QList<SearchDirectory>& Library::getSearchDirectories() const {
    return searchDirectories;
}

bool Library::addSearchDirectory(SearchDirectory dir) {
    if (getSearchDirectory(dir.dir.absolutePath()) == NULL) {
        searchDirectories.append(dir);
        return true;
    }
    return false;
}

SearchDirectory *Library::getSearchDirectory(const QString path) {
    for (int i=0; i < searchDirectories.length(); ++i) {
        if (searchDirectories.at(i).dir.absolutePath() == QDir(path).absolutePath()) {
            return &searchDirectories[i];
        }
    }
    return NULL;
}

bool Library::removeSearchDirectory(QString path) {
    for (int i=0; i < searchDirectories.length(); ++i) {
        if (searchDirectories.at(i).dir.absolutePath() == QDir(path).absolutePath()) {
            searchDirectories.removeAt(i);
            return true;
        }
    }
    return false;
}

void Library::generateFrenchises() {
    foreach (TvShow* show, tvShows) {
        show->syncRelations(*this);
    }
    foreach (const TvShow* show, tvShows) {
        addToFrenchise(show);
    }
    foreach (Franchise* franchise, franchises) {
        franchise->generateName();
    }
}

void Library::addToFrenchise(const TvShow* show) {
    bool added = false;
    foreach (Franchise* f, franchises) {
        if (f->hasRelationTo(show)) {
            f->addTvShow(show);
            added = true;
            break;
        }
    }
    if (!added) {
        Franchise* newFranchise = new Franchise(this);
        newFranchise->addTvShow(show);
        franchises.push_back(newFranchise);
    }
}

void Library::fetchingFinished() {
    qDebug() << "finished mal fetching, writing things now";
    generateFrenchises();
    this->write();
    qDebug() << "writing done!";
}

void Library::wallpaperDownloaderFinished() {
    runningWallpaperDownloaders.removeOne(dynamic_cast<WallpaperDownload::FetchThread*>(sender()));
    if (!getWallpaperDownloadRunning()) {
        emit wallpaperDownloadersFinished();
    }
}

void Library::readAll() {
    QString filepath = directory.absoluteFilePath("library.json");
    if (QFile(filepath).exists()) {
        nw::JsonReader jr(filepath.toStdString());


        jr.describeArray("searchDirectories", "directory", searchDirectories.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            QString dirpath;
            bool enabled = false;
            NwUtils::describe(jr, "path", dirpath);
            NwUtils::describe(jr, "enabled", enabled);
            searchDirectories.append(SearchDirectory(dirpath, enabled));
        }

        jr.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            std::string name;
            jr.describeValue(name);
            TvShow& show = tvShow(QString(name.data()));

            QDir showDir = show.directory(directory);
            if (!showDir.exists()) {
                qDebug() << "show does not have a directory: " << show.name();
                break;
            }
            show.read(showDir);
        }
        jr.close();
    } else {
        QStringList defaultDirs = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
        defaultDirs.append(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation));

        for (int i=0; i < defaultDirs.length(); ++i) {
            QDir dir = QDir(defaultDirs.at(i));
            if (dir.exists()) {
                searchDirectories.append(SearchDirectory(dir));
            }
        }
    }
}

void Library::write() {
    if (directory.exists()) {
        nw::JsonWriter jw(directory.absoluteFilePath("library.json").toStdString());
        jw.describeArray("searchDirectories", "directory", searchDirectories.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            QString dirpath = searchDirectories.at(i).dir.path();
            NwUtils::describe(jw, "path", dirpath);
            NwUtils::describe(jw, "enabled", searchDirectories[i].enabled);
        }
        jw.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = tvShows[i];
            std::string name = show->name().toStdString();
            jw.describeValue(name);

            QDir showDir = show->directory(directory);
            if (!showDir.exists() && !directory.mkdir(showDir.dirName())) {
                // TODO
                qDebug() << "TODO thow error can not write library" << showDir.absolutePath();
                continue;
            }
            nw::JsonWriter showJw(showDir.absoluteFilePath("tvShow.json").toStdString());
            show->write(showJw);
            showJw.close();
        }
        jw.close();
    }
}

QDir Library::getDirectory() const
{
    return directory;
}
