#include "library.h"
#include <N0Slib.h>
#include <QDebug>

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path)
{
}

void Library::initMalClient(QString malConfigFilepath) {
    if (QFile(malConfigFilepath).exists()) {
        std::string user, password;

        nw::JsonReader jr(malConfigFilepath.toStdString());
        jr.describe("user", user);
        jr.describe("password", password);
        jr.close();

        if (user.length() > 0 && password.length() > 0) {
            qDebug() << "mal connection is " << malClient.setCredentials(QString(user.data()), QString(password.data()));
        }
    }
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
