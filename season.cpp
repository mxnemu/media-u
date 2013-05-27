#include "season.h"
#include <iostream>

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
    std::cout << episodes.back()->releaseGroup().toStdString() << episodes.back()->name().toStdString() << std::endl;
}

QString Season::name() const {
    return mName;
}
