#include "franchise.h"
#include "utils.h"

Franchise::Franchise(QObject *parent) :
    QObject(parent)
{
}

bool Franchise::hasRelationTo(const TvShow *show) const {
    foreach (const TvShow* s, tvShows) {
        if (s == show || s->hasRelationTo(show)) {
            return true;
        }
    }
    return false;
}

void Franchise::addTvShow(const TvShow* show) {
    if (tvShows.contains(show)) {
        return;
    }
    tvShows.push_back(show);
}

void Franchise::generateName() {
    if (tvShows.length() == 1) {
        this->name = tvShows.front()->name();
        emit nameGenerated();
        return;
    }

    QStringList titles;
    foreach(const TvShow* show, tvShows) {
        titles << show->name();
    }

    this->name = Utils::commonSliceInStrings(titles);
    emit nameGenerated();
}
