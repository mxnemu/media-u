#include "library.h"

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path)
{
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



void Library::importTvShowEpisode(QString episodePath) {
    MovieFile episode(episodePath);
    TvShow& show = this->tvShow(episode.showName());
    show.importEpisode(episode);
}
