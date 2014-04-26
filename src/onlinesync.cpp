#include "onlinesync.h"
#include "malcredentials.h"
#include "malclient.h"
#include "maltracker.h"

OnlineSync::OnlineSync() {

}

void OnlineSync::init(QString configFile) {
    MalCredentials* malCreds = new MalCredentials();
    this->credentials.push_back(malCreds);
    this->databases.push_back(new Mal::Client(*malCreds, this));
    this->trackers.push_back(new Mal::Tracker(*malCreds, this));
}

void OnlineSync::fetchShows(QList<TvShow*> shows, Library& library) {
    for (TvShow* show : shows) {
        QList<OnlineTvShowDatabase::SearchResult> results;
        for (OnlineTvShowDatabase::Client* db : databases) {
            db->findShow(*show, library);
        }
        /* old update code from Client
            if (!result) {
                return false;
            }
            const Entry* entry = this->bestResult(*result);
            if (entry) {
                entry->updateShow(show, library);
                return true;
            }
            return false;
        */

    }
    emit databasesFinished();
    emit trackersFinished();
    emit allFinished();
}
