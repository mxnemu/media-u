#include "library.h"
#include "nwutils.h"
#include <QDebug>

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path),
    mFilter(tvShows)
{
    if (!directory.exists() && !QDir::root().mkpath(directory.absolutePath())) {
        qDebug() << "could not create library dir";
    }
}

void Library::initMalClient(QString malConfigFilepath) {
    malClient.init(malConfigFilepath);
}

QString Library::randomWallpaperPath() const {
    // TODO debug test file; Impl actual fn
    return QString("/home/nehmulos/Downloads/test-wall.jpg");
}

TvShow& Library::tvShow(QString name) {
    for (QList<TvShow>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if (it->name() == name) {
            return it.i->t();
        }
    }
    this->tvShows.push_back(TvShow(name));
    return this->tvShows.back();
}

LibraryFilter &Library::filter()
{
    return mFilter;
}

void Library::importTvShowEpisode(QString episodePath) {
    MovieFile episode(episodePath);
    TvShow& show = this->tvShow(episode.showName());
    show.importEpisode(episode);
}

void Library::fetchMetaData() {
    malClient.fetchShows(tvShows);
    connect(&malClient, SIGNAL(fetchingFinished()),
            this, SLOT(fetchingFinished()));
}

void Library::fetchingFinished() {
    qDebug() << "finished mal fetching, writing things now";
    this->write();
    qDebug() << "writing done!";
}

void Library::readAll() {
    if (directory.exists()) {
        nw::JsonReader jr(directory.absoluteFilePath("library.json").toStdString());
        jr.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            std::string name;
            jr.describeValue(name);
            TvShow& show = tvShow(QString(name.data()));

            QDir showDir(directory.absoluteFilePath(show.name()));
            if (!showDir.exists()) {
                qDebug() << "show does not have a directory: " << show.name();
                break;
            }
            show.read(showDir);
        }
        jr.close();
    }
}

void Library::write() {
    if (directory.exists()) {
        nw::JsonWriter jw(directory.absoluteFilePath("library.json").toStdString());
        jw.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow& show = tvShows[i];
            std::string name = show.name().toStdString();
            jw.describeValue(name);

            QDir showDir(directory.absoluteFilePath(show.name()));
            if (!showDir.exists() && !directory.mkdir(show.name())) {
                // TODO
                qDebug() << "TODO thow error can not write library";
                break;
            }
            show.write(showDir);
        }
        jw.close();
    }
}
