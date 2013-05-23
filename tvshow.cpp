#include "tvshow.h"

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

void TvShow::importEpisode(const MovieFile &episode) {
    Season& season = this->season(episode.seasonName());
    season.addEpisode(episode);
}



QString TvShow::name() const {
    return mName;
}
