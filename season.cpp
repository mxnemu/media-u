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

void Season::writeAsElement(nw::JsonWriter &jw) const {
    jw.describeValueArray("episodes", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        const MovieFile* episode = episodes.at(i);
        std::string episodePath = episode->path().toStdString();
        jw.describeValue(episodePath);
    }
}

void Season::addEpisode(const MovieFile& file) {
    episodes.append(new MovieFile(file.path()));
    qDebug() << episodes.back()->releaseGroup()
             << episodes.back()->episodeNumber()
             << episodes.back()->showName()
             << episodes.back()->seasonName()
             << episodes.back()->path();
}

QString Season::name() const {
    return mName;
}
