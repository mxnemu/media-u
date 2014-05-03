#include "onlinesync.h"
#include "malcredentials.h"
#include "malclient.h"
#include "maltracker.h"

OnlineSync::OnlineSync() {
    connect(this, SIGNAL(databasesFinished()), this, SLOT(checkIfAllFinished()));
    connect(this, SIGNAL(trackersFinished()), this, SLOT(checkIfAllFinished()));
}

void OnlineSync::init(QString configFile) {
    MalCredentials* malCreds = new MalCredentials();
    this->credentials.push_back(malCreds);
    this->databases.push_back(new Mal::Client(*malCreds, this));
    this->trackers.push_back(new Mal::Tracker(*malCreds, this));
}

void OnlineSync::fetchShow(TvShow* show, Library& library) {
    QList<OnlineTvShowDatabase::SearchResult*> results;
    for (OnlineTvShowDatabase::Client* db : databases) {
        results.push_back(db->findShow(*show));
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
    emit databasesFinished();
}


void OnlineSync::updateShow(TvShow* show) {
    this->unhandledFetch.insert(show);
    bool noFail = true;
    for (OnlineTracker* tracker : trackers) {
        bool success = tracker->updateRemote(show);
        noFail *= success;
    }
    if (noFail) {
        this->unhandledFetch.erase(show);
        emit trackersFinished();
    }
}


void OnlineSync::checkIfAllFinished() {
    if (unhandledFetch.empty() && unhandledUpdate.empty()) {
        emit allFinished();
    }
}
