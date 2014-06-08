#include "onlinetvshowdatabase.h"
#include <QDebug>
#include <utils.h>
#include "library.h"

namespace OnlineTvShowDatabase {
Client::Client(OnlineCredentials& credentials, QObject* parent) :
    QObject(parent),
    credentials(credentials)
{
}

SearchResult* Client::findShow(TvShow& show) {
    QString name = show.name();
    SearchResult* result = this->search(name);

    if (!result) {
        // TODO try alternate name
    }

    return result;
}

SearchResult::SearchResult(QString searchedQuery) : searchedQuery(searchedQuery) {}

SearchResult::~SearchResult() {
    foreach(Entry* entry, entries) {
        delete entry;
    }
}

Entry::Entry() {}
Entry::~Entry() {}

void Entry::updateShow(TvShow& show, const Library& library, const QString identifierKey, UpdateFilter filter) const {

    const TvShow* existingShow = library.filter().getShowForRemoteId(identifierKey, this->getRemoteId());

    if (filter & OnlineTvShowDatabase::ufSynopsis) {
        updateSynopsis(show);
    }

    if (filter & OnlineTvShowDatabase::ufTitle) {
        // TODO add title aliases and enable this,
        // make sure new files will check synonyms list when adding to library
        //updateTitle(show);
    }

    if (filter & OnlineTvShowDatabase::ufRelations) {
        updateRelations(show);
    }

    if (filter & OnlineTvShowDatabase::ufAiringDates) {
        updateAiringDates(show);
    }

    // never overwrite remoteId and synonyms when there is a conflicting entry
    if (!existingShow || existingShow == &show) {
        if (filter & OnlineTvShowDatabase::ufSynonyms) {
            updateSynonyms(show);
        }

        if (filter & OnlineTvShowDatabase::ufRemoteId) {
            updateRemoteId(show);
        }
    }

    if (filter & OnlineTvShowDatabase::ufImage) {
        updateImage(show, library.getDirectory());
    }

    //qDebug() << "updated show from mal-api.com" << id << title;
}

}
