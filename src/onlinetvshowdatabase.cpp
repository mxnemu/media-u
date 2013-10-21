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
    SearchResult result = search(name);
    const Entry* entry = result.bestResult();
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

SearchResult Client::search(QString anime) {
    CurlResult userData;
    QString url = QString("http://mal-api.com/anime/search?q=%1").arg(anime);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        SearchResult result(userData, anime);
        return result;
    }
    return SearchResult();
}

CURL* Client::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}


SearchResult::SearchResult() {}

SearchResult::SearchResult(CurlResult &response, QString searchedAnime) :
    searchedAnime(searchedAnime)
{
    qDebug() << "json" << response.data.str().data();
    nw::JsonReader jr(response.data);
    this->describe(&jr);
    jr.close();
}

void SearchResult::describe(nw::Describer* const de) {
    de->describeArray("", "entry", 0);
    for (int i=0; de->enterNextElement(i); ++i) {
        entries.push_back(Entry(de));
    }
}

const Entry* SearchResult::bestResult() {
    std::pair<int, const Entry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        const Entry* entry = &entries.at(i);

        int score = Utils::querySimiliarity(this->searchedAnime, entry->title);
        foreach (const QString& name, entry->englishTitles) {
            int s = Utils::querySimiliarity(this->searchedAnime, name);
            score = score >= s ? score : s;
        }
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(this->searchedAnime, name);
            score = score >= s ? score : s;
        }
        foreach (const QString& name, entry->japaneseTitles) {
            int s = Utils::querySimiliarity(this->searchedAnime, name);
            score = score >= s ? score : s;
        }
        if (score > best.first) {
            best.first = score;
            best.second = entry;
        }
    }
    return best.second;
}

Entry::Entry(nw::Describer *de) {
    describe(de);
}

void Entry::describe(nw::Describer *de) {
    NwUtils::describe(*de, "id", id);

    NwUtils::describe(*de, "title", title);
    //other_titles - A hash/dictionary containing other titles this anime has.
    NwUtils::describeValueArray(*de, "synonyms",synonyms);
    NwUtils::describeValueArray(*de, "englishTitles",englishTitles);
    NwUtils::describeValueArray(*de, "japaneseTitles", japaneseTitles);

    NwUtils::describe(*de, "image_url", image_url);
    NwUtils::describe(*de, "type", type);
    NwUtils::describe(*de, "episodes", episodes);
    NwUtils::describe(*de, "status", status);
    NwUtils::describe(*de, "start_date", start_date);
    NwUtils::describe(*de, "end_date", end_date);
    NwUtils::describe(*de, "classification", classification);
    NwUtils::describe(*de, "synopsis", synopsis);
    NwUtils::describeValueArray(*de, "genres", genres);
    NwUtils::describeValueArray(*de, "tags", tags);
    RelatedTvShow::parseFromList(de, "manga_adaptations", manga_adaptations, false);
    RelatedTvShow::parseFromList(de, "prequels", prequels, true);
    RelatedTvShow::parseFromList(de, "sequels", sequels, true);
    RelatedTvShow::parseFromList(de, "side_stories", side_stories, true);
    //check for parentstory == null first
    //parent_story.parseForAnime(de);
    RelatedTvShow::parseFromList(de, "character_anime", character_anime, true);
    RelatedTvShow::parseFromList(de, "spin_offs", spin_offs, true);
    RelatedTvShow::parseFromList(de, "summaries", summaries, true);
    RelatedTvShow::parseFromList(de, "alternative_versions", alternative_versions, true);
}

void Entry::updateShow(TvShow& show, QDir& libraryDir, UpdateFilter filter) const {

    //show.setName(title);
    show.setTotalEpisodes(episodes);
    show.setShowType(type);
    show.setRemoteId(id);

    if (filter & OnlineTvShowDatabase::ufSynonyms) {
        show.addSynonyms(synonyms);
        show.addSynonyms(englishTitles);
        show.addSynonyms(japaneseTitles);
    }

    if (filter & OnlineTvShowDatabase::ufAiringDates) {
        show.setAiringStatus(status);
        show.setStartDate(QDate::fromString(start_date, Entry::dateFormat));
        show.setEndDate(QDate::fromString(end_date, Entry::dateFormat));
    }

    if (filter & OnlineTvShowDatabase::ufSynopsis) {
        show.setSynopsis(synopsis);
    }

    if (filter & OnlineTvShowDatabase::ufRelations) {
        show.addPrequels(prequels);
        show.addPrequels(QList<RelatedTvShow>() << parent_story);

        show.addSideStories(side_stories);
        show.addSideStories(character_anime);
        show.addSideStories(spin_offs);
        show.addSideStories(summaries);

        show.addSequels(sequels);
    }

    show.downloadImage(image_url, libraryDir);

    qDebug() << "updated show from mal-api.com" << id << title;
}

QString Entry::dateFormat = "yyyy-MM-dd";


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
