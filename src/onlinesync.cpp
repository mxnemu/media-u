#include "onlinesync.h"
#include "malcredentials.h"
#include "malclient.h"
#include "maltracker.h"

OnlineSync::OnlineSync() {
    connect(this, SIGNAL(databasesFinished()), this, SLOT(checkIfAllFinished()));
    connect(this, SIGNAL(trackersFinished()), this, SLOT(checkIfAllFinished()));
}

void OnlineSync::init(QString configFile) {
    // TODO use a library that accesses system keychains
    MalCredentials* malCreds = new MalCredentials();
    malCreds->readConfig(configFile);
    this->credentials.push_back(malCreds);
    this->databases.push_back(new Mal::Client(*malCreds, this));
    this->trackers.push_back(new Mal::Tracker(*malCreds, this));
}

void OnlineSync::fetchShow(TvShow* show, Library& library) {
    // TODO combine all results and use the metaData from the best one
    //QList<OnlineTvShowDatabase::SearchResult*> results;
    this->unhandledFetch.insert(show);
    bool anySuccess = false;
    for (OnlineTvShowDatabase::Client* db : databases) {

        TODO continue coding here
                check if it actually needs to be (re)fetched

        OnlineTvShowDatabase::SearchResult* result = db->findShow(*show);

        if (!result) {
            continue;
        }

        //results.push_back(result);
        const OnlineTvShowDatabase::Entry* entry = db->bestResult(*result);
        if (entry) {
            entry->updateShow(*show, library);
            anySuccess = true;
            break;
        }
    }
    if (anySuccess) {
        this->unhandledFetch.erase(show);
        emit databasesFinished();
    }
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
