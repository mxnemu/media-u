#include "season.h"
#include <QDebug>

Season::Season(QString name) {
    mName = name;
}

Season::~Season()
{
    foreach (MovieFile* file, episodes) {
        if (file) {
            delete file;
        }
    }
}

void Season::addEpisode(const MovieFile& file) {
    episodes.append(new MovieFile(file.path()));
    qDebug() << episodes.back()->releaseGroup()
             << episodes.back()->episodeNumber()
             << episodes.back()->name()
             << episodes.back()->seasonName()
             << episodes.back()->path();
}

QString Season::name() const {
    return mName;
}
