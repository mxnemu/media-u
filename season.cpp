#include "season.h"

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
}

QString Season::name() const {
    return mName;
}
