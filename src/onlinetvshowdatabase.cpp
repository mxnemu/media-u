#include "onlinetvshowdatabase.h"
#include <QDebug>
#include <utils.h>

namespace OnlineTvShowDatabase {
Client::Client(QObject* parent) :
    QObject(parent),
    activeThread(NULL)
{
}


void Client::startUpdate(QList<TvShow*> &showList, QDir libraryDir) {
    if (this->activeThread) {
        return;
    }
    this->activeThread = new Thread(*this, showList, libraryDir, this);
    connect(this->activeThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    this->activeThread->start();
}

bool Client::updateShow(TvShow& show, QDir &libraryDir) {
    QString name = show.name();
    SearchResult result = this->search(name);
    const Entry* entry = this->bestResult(result);
    if (entry) {
        entry->updateShow(show, libraryDir);
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

void Entry::updateShow(TvShow& show, QDir& libraryDir, UpdateFilter filter) const {

    if (filter & OnlineTvShowDatabase::ufSynonyms) {
        updateSynonyms(show);
    }

    if (filter & OnlineTvShowDatabase::ufAiringDates) {
        updateAiringDates(show);
    }

    if (filter & OnlineTvShowDatabase::ufSynopsis) {
        updateSynopsis(show);
    }

    if (filter & OnlineTvShowDatabase::ufRelations) {
        updateRelations(show);
    }

    if (filter & OnlineTvShowDatabase::ufImage) {
        updateImage(show, libraryDir);
    }

    //qDebug() << "updated show from mal-api.com" << id << title;
}


Thread::Thread(Client &client, QList<TvShow*> &shows, QDir libraryDir, QObject *parent) :
    QThread(parent),
    client(client),
    tvShows(shows),
    libraryDir(libraryDir)
{
}

void Thread::run() {
    foreach (TvShow* show, tvShows) {
        if (show) {
            client.updateShow(*show, libraryDir);
        }
    }
}

}
