#include "onlinetvshowdatabase.h"
#include <QDebug>
#include <utils.h>
#include "library.h"

namespace OnlineTvShowDatabase {
Client::Client(QObject* parent) :
    QObject(parent),
    activeThread(NULL)
{
}


void Client::startUpdate(QList<TvShow*> &showList, const Library& library) {
    if (this->activeThread) {
        return;
    }
    this->activeThread = new Thread(*this, showList, library, this);
    connect(this->activeThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    this->activeThread->start();
}

bool Client::findShow(TvShow& show, const Library& library) {
    QString name = show.name();
    SearchResult* result = this->search(name);
    if (!result) {
        return false;
    }
    const Entry* entry = this->bestResult(*result);
    if (entry) {
        entry->updateShow(show, library);
        return true;
    }
    return false;
}

void Client::threadFinished() {
    if (dynamic_cast<Thread*>(sender()) != this->activeThread) {
        throw "malapidotcom::Client::threadFinished called from unknown thread";
    }
    this->activeThread = NULL;
    emit updateFinished();
}

SearchResult::SearchResult(QString searchedQuery) : searchedQuery(searchedQuery) {}

SearchResult::~SearchResult() {
    foreach(Entry* entry, entries) {
        delete entry;
    }
}

Entry::Entry() {}
Entry::~Entry() {}

void Entry::updateShow(TvShow& show, const Library& library, UpdateFilter filter) const {

    const TvShow* existingShow = library.filter().getShowForRemoteId(this->getRemoteId());

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


Thread::Thread(Client &client, QList<TvShow*> &shows, const Library& library, QObject *parent) :
    QThread(parent),
    client(client),
    tvShows(shows),
    library(library),
    requestSleepPadding(3000)
{
}

void Thread::run() {

    QTime loginTimer;
    loginTimer.start();

    if (!client.login()) {
        qDebug() << "can't fetch data, no valid login credentials";
        return;
    }
    int loginSleep = requestSleepPadding - loginTimer.elapsed();
    if (loginSleep > 0) {
        msleep(loginSleep);
    }

    bool fetchingSucess = client.fetchOnlineTrackerList(tvShows);

    QDate now = QDate::currentDate();

    foreach (TvShow* show, tvShows) {
        if (!show) {
            continue;
        }

        if (show->getRemoteId() == -1 ||
            show->getTotalEpisodes() == 0 ||
            (show->isAiring() && (
             (show->episodeList().numberOfEpisodes() >= show->getTotalEpisodes()) ||
             (show->getEndDate().isValid() && now > show->getEndDate()) ||
             (show->getStatus() == TvShow::completed)))) {

            QTime timer;
            timer.start();

            client.findShow(*show, library);

            int sleepTime = this->requestSleepPadding - timer.elapsed();
            if (sleepTime > 0) {
                msleep(sleepTime);
            }
        }

        if (fetchingSucess) {
            client.updateInOnlineTracker(show);
        }
    }


}

}
