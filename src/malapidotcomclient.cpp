#include "malapidotcomclient.h"
#include <QDebug>
#include "utils.h"

namespace MalApiDotCom {

Client::Client(OnlineCredentials& credentials, QObject* parent) :
    OnlineTvShowDatabase::Client(credentials, parent)
{
}

SearchResult* Client::search(QString anime) {
    CurlResult userData;
    QString url = QString("http://mal-api.com/anime/search?q=%1").arg(anime);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        return parseSearch(userData, anime);
    }
    return NULL;
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

const Entry* Client::bestResult(const SearchResult &sr) const {
    std::pair<int, const Entry*> best(-1, NULL);
    for (int i=0; i < sr.entries.length(); ++i) {
        const Entry* entry = dynamic_cast<const Entry*>(sr.entries.at(i));

        int score = Utils::querySimiliarity(sr.searchedQuery, entry->title);
        foreach (const QString& name, entry->englishTitles) {
            int s = Utils::querySimiliarity(sr.searchedQuery, name);
            score = score >= s ? score : s;
        }
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(sr.searchedQuery, name);
            score = score >= s ? score : s;
        }
        foreach (const QString& name, entry->japaneseTitles) {
            int s = Utils::querySimiliarity(sr.searchedQuery, name);
            score = score >= s ? score : s;
        }
        if (score > best.first) {
            best.first = score;
            best.second = entry;
        }
    }
    return best.second;
}

SearchResult* Client::parseSearch(CurlResult &response, QString searchedAnime) {
    nw::JsonReader jr(response.data);
    SearchResult* result = new SearchResult(searchedAnime);
    jr.describeArray("", "entry", 0);
    for (int i=0; jr.enterNextElement(i); ++i) {
        result->entries.push_back(new Entry(&jr));
    }
    jr.close();
    return result;
}

Entry::Entry(nw::Describer* de) : OnlineTvShowDatabase::Entry() {
    describe(de);
}

int Entry::getRemoteId() const {
    return id;
}

void Entry::updateSynopsis(TvShow &show) const {
    show.setSynopsis(synopsis);
}

void Entry::updateTitle(TvShow &) const {
    //show.setName(title);
}

void Entry::updateRemoteId(TvShow &show) const {
    show.setRemoteId(id);
}

void Entry::updateRelations(TvShow &show) const {
    show.addPrequels(prequels);
    show.addPrequels(QList<RelatedTvShow>() << parent_story);

    show.addSideStories(side_stories);
    show.addSideStories(character_anime);
    show.addSideStories(spin_offs);
    show.addSideStories(summaries);

    show.addSequels(sequels);
}

void Entry::updateAiringDates(TvShow &show) const {
    show.setTotalEpisodes(episodes);
    show.setShowType(type);
    show.setAiringStatus(status);
    show.setStartDate(QDate::fromString(start_date, Entry::dateFormat));
    show.setEndDate(QDate::fromString(end_date, Entry::dateFormat));
}

void Entry::updateSynonyms(TvShow &show) const {
    show.addSynonyms(synonyms);
    show.addSynonyms(englishTitles);
    show.addSynonyms(japaneseTitles);
}

void Entry::updateImage(TvShow &show, QDir libraryDir) const {
    show.downloadImage(image_url, libraryDir);
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


QString Entry::dateFormat = "yyyy-MM-dd";

} // namespace
