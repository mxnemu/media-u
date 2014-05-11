#include "onlinesync.h"
#include "malcredentials.h"
#include "malclient.h"
#include "maltracker.h"
#include "config.h"

OnlineSync::OnlineSync(const Library& library) :
    library(library)
{
    connect(this, SIGNAL(databasesFinished()), this, SLOT(checkIfAllFinished()));
    connect(this, SIGNAL(trackersFinished()), this, SLOT(checkIfAllFinished()));
}

void OnlineSync::init(const BaseConfig& config) {
    // TODO use a library that accesses system keychains
    MalCredentials* malCreds = new MalCredentials();
    malCreds->readConfig(config.malConfigFilePath());
    this->credentials.push_back(malCreds);
    this->databases.push_back(new Mal::Client(*malCreds, this));
    this->trackers.push_back(new Mal::Tracker(*malCreds, this));
}

void OnlineSync::startThreadIfNotRunning() {
    if (!this->isRunning()) {
        this->start();
    }
}

void OnlineSync::addShowToFetch(TvShow *show) {
    this->unhandledFetch.insert(show);
    this->startThreadIfNotRunning();
}

void OnlineSync::addShowToUpdate(TvShow *show) {
    this->unhandledFetch.insert(show);
    this->startThreadIfNotRunning();
}

bool OnlineSync::requiresFetch(const TvShow* show, const QString dbIdentifier) {
    QDate today = QDate::currentDate();
    return
        show->getRemoteId(dbIdentifier) == -1 ||
        show->getTotalEpisodes() == 0 ||
        (show->isAiring() && (
         (show->episodeList().numberOfEpisodes() >= show->getTotalEpisodes()) ||
         (show->getEndDate().isValid() && today > show->getEndDate()) ||
         (show->getStatus() == TvShow::completed)));
}

bool OnlineSync::fetchShow(TvShow* show, const Library& library) {
    // TODO combine all results and use the metaData from the best one
    //QList<OnlineTvShowDatabase::SearchResult*> results;
    bool anySuccess = false;
    for (OnlineTvShowDatabase::Client* db : databases) {
        if (!requiresFetch(show, db->identifierKey())) {
            continue;
        }

        OnlineTvShowDatabase::SearchResult* result = db->findShow(*show);
        if (!result) {
            continue;
        }

        //results.push_back(result);
        const OnlineTvShowDatabase::Entry* entry = db->bestResult(*result);
        if (entry) {
            entry->updateShow(*show, library, db->identifierKey());
            anySuccess = true;
            // TODO don't break, collect all,
            // update metaData from the best entry of all
            // update remoteId for every db
            break;
        }
    }
    if (anySuccess) {
        return true;
    }
    return false;
}


bool OnlineSync::updateShow(TvShow* show) {
    bool noFail = true;
    for (OnlineTracker* tracker : trackers) {
        bool success = tracker->updateRemote(show);
        noFail *= success;
    }
    if (noFail) {
        return true;
    }
    return false;
}


void OnlineSync::checkIfAllFinished() {
    if (unhandledFetch.empty() && unhandledUpdate.empty()) {
        emit allFinished();
    }
}

void OnlineSync::run() {
    while (!unhandledFetch.empty()) {
        auto itr = unhandledFetch.begin();
        TvShow* show = *itr;
        bool success = this->fetchShow(show, library);
        if (!success) {
            qDebug() << "failed to fetch" << show->name();
        }
        unhandledFetch.erase(itr);
    }
    emit databasesFinished();

    // TODO copy pasta
    while (!unhandledUpdate.empty()) {
        auto itr = unhandledUpdate.begin();
        TvShow* show = *itr;
        bool success = this->updateShow(show);
        if (!success) {
            qDebug() << "failed to update" << show->name();
        }
        unhandledUpdate.erase(itr);
    }
    emit trackersFinished();
}
