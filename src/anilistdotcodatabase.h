#ifndef ANILISTDOTCODATABASE_H
#define ANILISTDOTCODATABASE_H

#include "onlinetvshowdatabase.h"

class AnilistDotCoDatabase : public OnlineTvShowDatabase::Client
{
public:

    class ExtendedEntry {
//        "start_date": "1998-04-03T21:00:00+09:00",
//        "end_date": "1999-04-24T21:00:00+09:00",
//        "classification": "R - 17+ (violence & profanity)",
//        "hashtag": null,
//        "source": null,
//        "description": "Enter a world in the distant future, where Bounty Hunters roam the solar system. Spike and Jet, bounty hunting partners, set out on journeys in an ever struggling effort to win bounty rewards to survive.<br><br>\nWhile traveling, they meet up with other very interesting people. Could Faye, the beautiful and ridiculously poor gambler, Edward, the computer genius, and Ein, the engineered dog be a good addition to the group?",
//        "genres": [
//                "Action",
//                "Adventure",
//                "Comedy",
//                "Drama",
//                "Sci-Fi",
//                "Space"
//        ],
//        "image_url_banner": "http://anilist.co/img/dir/anime/banner/1.jpg",
//        "duration": 24,
//        "youtube_id": null,
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
    public:
        ExtendedEntry();

        void describe(nw::Describer& de);

        QDateTime start_date; // "1998-04-03T21:00:00+09:00",
        QDateTime end_date; // "1999-04-24T21:00:00+09:00",
        QString classification; // "R - 17+ (violence & profanity)",
        QString hashtag; // null,
        QString source; // null,
        QString description; // long string
        QStringList genres; // [strings]
        QString image_url_banner; // "http://anilist.co/img/dir/anime/banner/1.jpg",
        int duration; // 24,
        QString youtube_id; // null,
        //QList<int> list_stats; // {
        //int airing; // {
        bool isAiring;
    };

    class SearchResult : public OnlineTvShowDatabase::SearchResult {
    public:
        SearchResult(CurlResult& userdata, const OnlineCredentials& credentials);

        virtual OnlineTvShowDatabase::Entry* bestEntry();
    private:
        const OnlineCredentials& credentials;
    };

    class Entry : public OnlineTvShowDatabase::Entry {
    public:
        friend class SearchResult;
        Entry();
        virtual ~Entry();
        virtual int getRemoteId() const;
        virtual void updateSynopsis(TvShow& show) const;
        virtual void updateTitle(TvShow& show) const;
        virtual void updateRemoteId(TvShow& show) const;
        virtual void updateRelations(TvShow& show) const;
        virtual void updateAiringDates(TvShow& show) const;
        virtual void updateSynonyms(TvShow& show) const;
        virtual void updateImage(TvShow& show, QDir libraryDir) const;

        void describe(nw::Describer& de);
        void fetchExtended(const OnlineCredentials &credentials);

        int id; // 1,
        QString title_romaji; // "Cowboy Bebop",
        QString type; // "TV",
        QString image_url_med; // "http://anilist.co/img/dir/anime/med/1.jpg",
        QString image_url_sml; // "http://anilist.co/img/dir/anime/sml/1.jpg",
        QString title_japanese; // "カウボーイビバップ",
        QString title_english; // "Cowboy Bebop",
        QStringList synonyms; // [],
        QString image_url_lge; // "http://anilist.co/img/dir/anime/reg/1.jpg",
        QString airing_status; // "finished airing",
        QString average_score; // "86.8",
        int total_episodes; // 26,
        bool adult; // false,
        int popularity; // 7574,
        int relation_type; // null,
        int role; // null

        ExtendedEntry* extended;
    };

    AnilistDotCoDatabase(OnlineCredentials &credentials, QObject *parent);
    virtual const QString identifierKey() const;
    virtual OnlineTvShowDatabase::UpdateFilter getFilter() { return OnlineTvShowDatabase::UpdateFilter::ufAll; }

protected:
    virtual OnlineTvShowDatabase::SearchResult* search(QString anime);
private:
    void describeUrl(nw::Describer &de, const nw::String key, QString &value);
};

#endif // ANILISTDOTCODATABASE_H
