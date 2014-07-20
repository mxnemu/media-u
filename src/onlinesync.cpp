#include "onlinesync.h"
#include "malcredentials.h"
#include "malclient.h"
#include "maltracker.h"
#include "config.h"

OnlineSync::OnlineSync(const Library& library) :
    shouldQuit(false),
    library(library)
{
}

void OnlineSync::init(const BaseConfig& config) {
    // TODO use a library that accesses system keychains
    MalCredentials* malCreds = new MalCredentials();
    malCreds->readConfig(config.malConfigFilePath());
    this->credentials.push_back(malCreds);
    this->databases.push_back(new Mal::Client(*malCreds, this));
    this->trackers.push_back(new Mal::Tracker(*malCreds, this));

    malCreds->login();
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
    this->unhandledUpdate.insert(show);
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
            anySuccess = true;
            continue;
        }

        OnlineTvShowDatabase::SearchResult* result = db->findShow(*show);
        if (!result) {
            continue;
        }

        // un-const fuckery
        int indexOfCreds = this->credentials.indexOf((OnlineCredentials*)&db->credentials);
        if (indexOfCreds == -1) {
            continue;
        }
        // TODO don't block this thread until other locks are tested
        while (this->credentials.at(indexOfCreds)->lock.blockUntilReady()) {
            // waits the correct time as side-effect
        }

        //results.push_back(result);
        const OnlineTvShowDatabase::Entry* entry = result->bestEntry();
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
        if (show->getRemoteId(tracker->identifierKey()) <= 0) {
            continue;
        }

        // TODO make a common base for tracker and db, to avoid coppy pasta
        // un-const fuckery
        int indexOfCreds = this->credentials.indexOf((OnlineCredentials*)&tracker->credentials);
        if (indexOfCreds == -1) {
            continue;
        }
        // TODO don't block this thread until other locks are tested
        while (this->credentials.at(indexOfCreds)->lock.blockUntilReady()) {
            // waits the correct time as side-effect
        }

        bool success = tracker->updateRemote(show);
        noFail *= success;
    }
    if (noFail) {
        return true;
    }
    return false;
}


void OnlineSync::checkIfAllFinished() {
    if (!unhandledFetch.empty()) {
        fetchDatabases();
        return;
    }
    if (!unhandledUpdate.empty()) {
        updateTrackers();
        return;
    }
    shouldQuit = true;
    disconnect(this, SIGNAL(databasesFinished()), this, SLOT(checkIfAllFinished()));
    disconnect(this, SIGNAL(trackersFinished()), this, SLOT(checkIfAllFinished()));
    emit allFinished();
}

void OnlineSync::fetchDatabases() {
    while (!unhandledFetch.empty()) {
        auto itr = unhandledFetch.begin();

        // I've got no fucking idea why .empty() doesn't work here,
        // but checking for == .end() fixes a crash
        if (itr == unhandledFetch.end()) {
            break;
        }

        TvShow* show = *itr;
        bool success = this->fetchShow(show, library);
        if (!success) {
            qDebug() << "failed to fetch" << show->name();
        }
        unhandledFetch.erase(itr);
    }
    emit databasesFinished();
}

void OnlineSync::updateTrackers() {
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

void OnlineSync::run() {
    connect(this, SIGNAL(databasesFinished()), this, SLOT(checkIfAllFinished()), Qt::DirectConnection);
    connect(this, SIGNAL(trackersFinished()), this, SLOT(checkIfAllFinished()), Qt::DirectConnection);

    this->fetchDatabases();
    // TODO I've got no idea if this works how I expect.
    // right now I've got the problem that the thread quits
    // even though there are still signals unhandled, at least I guess that.
    // I hope this prevents the thread from quiting during signal handling
    // doing multithreading with an unfamiliar framework sucks
    while (!shouldQuit) {
        this->msleep(50); // sleep 0.05 seconds
    }
}
