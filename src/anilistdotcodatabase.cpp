#include "anilistdotcodatabase.h"
#include "anilistdotcocredentials.h"
#include "utils.h"
#include <QUrl>
#include <QDebug>

AnilistDotCoDatabase::AnilistDotCoDatabase(OnlineCredentials& credentials, OnlineCredentials::TimeLock& lock, QObject* parent = NULL) :
    OnlineTvShowDatabase::Client(credentials, lock, parent)
{

}

const QString AnilistDotCoDatabase::identifierKey() const {
    return AnilistDotCoCredentials::IDENTIFIER_KEY;
}



// GET: anime/search/{query}
// SmallEntry
//{
//        "id": 1,
//        "title_romaji": "Cowboy Bebop",
//        "type": "TV",
//        "image_url_med": "http://anilist.co/img/dir/anime/med/1.jpg",
//        "image_url_sml": "http://anilist.co/img/dir/anime/sml/1.jpg",
//        "title_japanese": "カウボーイビバップ",
//        "title_english": "Cowboy Bebop",
//        "synonyms": [],
//        "image_url_lge": "http://anilist.co/img/dir/anime/reg/1.jpg",
//        "airing_status": "finished airing",
//        "average_score": "86.8",
//        "total_episodes": 26,
//        "adult": false,
//        "popularity": 7574,
//        "relation_type": null,
//        "role": null
//}
OnlineTvShowDatabase::SearchResult* AnilistDotCoDatabase::search(QString anime) {
    CurlResult userdata;
    QString escapedQuery = anime.replace("!", "\\!");
    QString url = QUrl(QString("https://anilist.co/api/anime/search/%1").arg(escapedQuery)).toString(QUrl::FullyEncoded);
    CURL* handle = credentials.curlClient(lock, url.toStdString().c_str(), userdata);

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);

    if (error) {
        qDebug() << "received error during search " << error << " with this message:\n";
        userdata.print();
        return NULL;
    }

    return new SearchResult(userdata, credentials, lock);
}

AnilistDotCoDatabase::Entry::Entry() :
    id(-1),
    total_episodes(-1),
    adult(false),
    popularity(-1),
    relation_type(-1),
    role(-1),
    extended(NULL)
{

}

AnilistDotCoDatabase::Entry::~Entry() {
    if (this->extended) {
        delete this->extended;
    }
}

int AnilistDotCoDatabase::Entry::getRemoteId() const {
    return this->id;
}

void AnilistDotCoDatabase::Entry::updateSynopsis(TvShow &show) const {
    if (this->extended) {
        show.setSynopsis(this->extended->description);
    }
}

void AnilistDotCoDatabase::Entry::updateTitle(TvShow &) const {
//    can't modifiy title yet TODO
}

void AnilistDotCoDatabase::Entry::updateRemoteId(TvShow &show) const {
    show.setRemoteId(AnilistDotCoCredentials::IDENTIFIER_KEY, this->id);
}

void AnilistDotCoDatabase::Entry::updateRelations(TvShow &) const {
    // TODO
}

void AnilistDotCoDatabase::Entry::updateAiringDates(TvShow &show) const {
    show.setTotalEpisodes(this->total_episodes);
    show.setShowType(this->type);
    show.setAiringStatus(this->airing_status);
    if (this->extended) {
        show.setStartDate(this->extended->start_date.date());
        show.setEndDate(this->extended->end_date.date());
    }
}

void AnilistDotCoDatabase::Entry::updateSynonyms(TvShow &show) const {
    QStringList list = this->synonyms; // TODO don't reappend the actual title
    list.append(this->title_english);
    list.append(this->title_japanese);
    list.append(this->title_romaji);
    show.setSynonyms(list);
}

void AnilistDotCoDatabase::Entry::updateImage(TvShow &show, QDir libraryDir) const {
    show.downloadImage(this->image_url_lge, libraryDir);
}

void AnilistDotCoDatabase::Entry::describe(nw::Describer &de) {
    NwUtils::describe(de, "id", id);
    NwUtils::describe(de, "title_romaji", title_romaji);
    NwUtils::describe(de, "type", type);
    NwUtils::describe(de, "image_url_med", image_url_med);
    NwUtils::describe(de, "image_url_sml", image_url_sml);
    NwUtils::describe(de, "title_japanese", title_japanese);
    NwUtils::describe(de, "title_english", title_english);
    NwUtils::describeValueArray(de, "synonyms", synonyms);
    NwUtils::describe(de, "image_url_lge", image_url_lge);
    NwUtils::describe(de, "airing_status", airing_status);
    NwUtils::describe(de, "average_score", average_score);
    NwUtils::describe(de, "total_episodes", total_episodes);
    NwUtils::describe(de, "adult", adult);
    NwUtils::describe(de, "popularity", popularity);
    NwUtils::describe(de, "relation_type", relation_type);
    NwUtils::describe(de, "role", role);
}

// GET: anime/{id}
// BigEntry
//{
//        "id": 1,
//        "title_romaji": "Cowboy Bebop",
//        "type": "TV",
//        "image_url_med": "http://anilist.co/img/dir/anime/med/1.jpg",
//        "image_url_sml": "http://anilist.co/img/dir/anime/sml/1.jpg",
//        "start_date": "1998-04-03T21:00:00+09:00",
//        "end_date": "1999-04-24T21:00:00+09:00",
//        "classification": "R - 17+ (violence & profanity)",
//        "hashtag": null,
//        "source": null,
//        "title_japanese": "カウボーイビバップ",
//        "title_english": "Cowboy Bebop",
//        "synonyms": [],
//        "description": "Enter a world in the distant future, where Bounty Hunters roam the solar system. Spike and Jet, bounty hunting partners, set out on journeys in an ever struggling effort to win bounty rewards to survive.<br><br>\nWhile traveling, they meet up with other very interesting people. Could Faye, the beautiful and ridiculously poor gambler, Edward, the computer genius, and Ein, the engineered dog be a good addition to the group?",
//        "genres": [
//                "Action",
//                "Adventure",
//                "Comedy",
//                "Drama",
//                "Sci-Fi",
//                "Space"
//        ],
//        "image_url_lge": "http://anilist.co/img/dir/anime/reg/1.jpg",
//        "image_url_banner": "http://anilist.co/img/dir/anime/banner/1.jpg",
//        "duration": 24,
//        "airing_status": "finished airing",
//        "average_score": "86.8",
//        "total_episodes": 26,
//        "youtube_id": null,
//        "adult": false,
//        "popularity": 7574,
//        "relation_type": null,
//        "role": null,
//        "list_stats": {
//                "plan_to_watch": 1673,
//                "watching": 421,
//                "completed": 4855,
//                "on_hold": 511,
//                "dropped": 114
//        },
//        "airing": {
//                "time": "2015-04-12T00:00:00+09:00",
//                "countdown": 497524,
//                "next_episode": 2
//        }
//}
void AnilistDotCoDatabase::Entry::fetchExtended(const OnlineCredentials& credentials, OnlineCredentials::TimeLock& lock) {
    if (this->id == -1) {
        return;
    }

    CurlResult userdata;
    QString url = QUrl(QString("https://anilist.co/api/anime/%1").arg(this->id)).toString(QUrl::FullyEncoded);
    CURL* handle = credentials.curlClient(lock, url.toStdString().c_str(), userdata);

    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);

    if (error) {
        qDebug() << "received error during full-anime-model fetch" << error << " with this message:\n";
        userdata.print();
        return;
    }

    nw::JsonReader jr(userdata.data);
    ExtendedEntry* e = new ExtendedEntry();
    e->describe(jr);
    jr.close();
    this->extended = e;
}

OnlineTvShowDatabase::Entry *AnilistDotCoDatabase::SearchResult::bestEntry() {
    std::pair<int, Entry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        Entry* entry = (Entry*)entries[i];

        // TODO FIXME XXX FUCK ING HEKKELL this is suck horrible redundant cod ewith malclient
        // just put it all into a list and give it to the highest part
        // monads would also come in handy here
        int score = Utils::querySimiliarity(this->searchedQuery, entry->title_romaji);
        const int engScore = Utils::querySimiliarity(this->searchedQuery, entry->title_english);
        const int jpScore = Utils::querySimiliarity(this->searchedQuery, entry->title_japanese);
        score = score >= engScore ? score : engScore;
        score = score >= jpScore ? score : jpScore;
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(this->searchedQuery, name);
            score = score >= s ? score : s;
        }
        if (score > best.first) {
            best.first = score;
            best.second = entry;
        }
    }

    if (best.second) {
        best.second->fetchExtended(credentials, lock);
    }

    return best.second;
}


AnilistDotCoDatabase::SearchResult::SearchResult(CurlResult& userdata, const OnlineCredentials &credentials, OnlineCredentials::TimeLock& lock) :
    credentials(credentials),
    lock(lock)
{
    userdata.print();
    nw::JsonReader jr(userdata.data);
    jr.describeArray("", "", -1);
    for (unsigned int i=0; jr.enterNextElement(i); ++i) {
        Entry* e = new Entry();
        e->describe(jr);
        this->entries.push_back(e);
    }
    jr.close();
}


AnilistDotCoDatabase::ExtendedEntry::ExtendedEntry() :
    duration(-1),
    isAiring(false)
{

}

void AnilistDotCoDatabase::ExtendedEntry::describe(nw::Describer &de) {
    NwUtils::describe(de, "start_date", start_date);
    NwUtils::describe(de, "end_date", end_date);
    NwUtils::describe(de, "classification", classification);
    NwUtils::describe(de, "hashtag", hashtag);
    NwUtils::describe(de, "source", source);
    NwUtils::describe(de, "description", description);
    NwUtils::describeValueArray(de, "genres", genres);
    NwUtils::describe(de, "image_url_banner", image_url_banner);
    NwUtils::describe(de, "duration", duration);
    NwUtils::describe(de, "youtube_id", youtube_id);

    //QList<int> list_stats; // {
    //int airing; // {
    if (de.conditionalPush("airing_status", true)) {
        isAiring = true;
        de.pop();
    }
}
