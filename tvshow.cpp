#include "tvshow.h"
#include <N0Slib.h>


TvShow::TvShow(QString name) {
    this->mName = name;
}

Season& TvShow::season(QString name) {
    for (QList<Season>::iterator it = seasons.begin(); it != seasons.end(); ++it) {
        if (it->name() == name) {
            return it.i->t();
        }
    }
    this->seasons.push_back(Season(name));
    return this->seasons.back();
}

void TvShow::write(QDir &dir) const {
    nw::JsonWriter jw(dir.absoluteFilePath("tvShow.json").toStdString());
    jw.describeArray("seasons", "season", seasons.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        const Season& season = seasons.at(i);
        season.writeAsElement(jw);
    }
    jw.close();
}

void TvShow::importEpisode(const MovieFile &episode) {
    Season& season = this->season(episode.seasonName());
    season.addEpisode(episode);
}



QString TvShow::name() const {
    return mName;
}
