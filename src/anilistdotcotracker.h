#ifndef ANILISTDOTCOTRACKER_H
#define ANILISTDOTCOTRACKER_H

#include "onlinetracker.h"
#include "anilistdotcodatabase.h"

class AnilistDotCoTracker : public OnlineTracker {
    Q_OBJECT
public:
    class Entry : public OnlineTracker::Entry {
    public:
        QString list_status; // "on-hold",
        int score; // 0,
        int priorty; // null,
        int rewatched; // null,
        int notes; // null,
        bool isPrivate; // null, (renamed field)
        QDateTime updated_time; // "2014-10-15T18:56:23+09:00",
        QDateTime added_time; // "2014-10-15T18:56:23+09:00",
        int score_raw; // 0,
        QList<int> advanced_rating_scores; // [int]
        int episodes_watched; // 11,
        int chapters_read; // null,
        int volumes_read; // null,
        int hidden_default; // null,
        QStringList custom_lists; // [str]
        // Not there when fetching raw
        // but I'm never fetching raw so it's not a pointer.
        AnilistDotCoDatabase::Entry anime;

        virtual QDateTime lastUpdate() const { return updated_time; }
        virtual int remoteId() const { return anime.id; }
        virtual int watchedEpisodes() const { return episodes_watched; }
        virtual int rewatchMarker() const { return -1; }
        virtual int rewatchCount() const { return rewatched; }
        virtual bool supportsRewatchMarker() const { return false; }
        virtual int totalEpisodes() const { return this->anime.total_episodes; }
        virtual TvShow::WatchStatus getStatusWouldSendIfSynced(TvShow::WatchStatus showStatus) const;
        virtual TvShow::WatchStatus watchStatus() const { return watchStatusFromString(this->list_status); }

        void describe(nw::Describer& de);
    };

    class EntryList : public OnlineTracker::EntryList {
    public:
        EntryList(nw::JsonReader& jr);
        virtual const OnlineTracker::Entry* get(const QString trackerIdentifierKey, const TvShow* show) const;
        virtual void describe(nw::Describer& de);

        const QDateTime fetchTime;
        QList<Entry> entries;
    private:
        void describeList(nw::Describer &de, nw::String listKey);
    };

    class User {
    public:
        int id; // 1,
        QString display_name; //: "Josh",
        int anime_time; //: 54067,
        int manga_chap; //: 587,
        QString about; // "str"
        int list_order; //: 0,
        int adult_content; //: true,
        int following; //: false,
        QString image_url_lge; //: "http://img.anilist.co/user/reg/1.png",
        QString image_url_med; //: "http://img.anilist.co/user/sml/1.png",
        QString image_url_banner; //: "http://i.imgur.com/ZHAUS4K.jpg",
        QString title_language; //: "romaji",
        int score_type; //: 4,
        QStringList custom_list_anime; //: [strs]
        QStringList custom_list_manga; //: [strs]
        QStringList advanced_rating; //: [strs],
        QStringList advanced_rating_names; //: [ strs ]
        int notifications; //: 0

        User();
        bool fetchCurrentlyLoggedInUser(const OnlineCredentials &credentials, OnlineCredentials::TimeLock &lock);
        void describe(nw::Describer& de);
    };

    AnilistDotCoTracker(OnlineCredentials& credentials, OnlineCredentials::TimeLock& lock, QObject *parent);
    virtual ~AnilistDotCoTracker();

    virtual const QString identifierKey() const;
    virtual OnlineTracker::EntryList* fetchRemote(); ///< Must create a new EntryList using online Data, or NULL on error

    static TvShow::WatchStatus watchStatusFromString(QString status);
    static QString watchStatusToString(TvShow::WatchStatus status);
protected:
    virtual UpdateResult updateinOnlineTrackerOrAdd(const TvShow* show, const QString& type) const;
private:
    User user; // required for the id
};

#endif // ANILISTDOTCOTRACKER_H
