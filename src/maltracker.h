#ifndef MALTRACKER_H
#define MALTRACKER_H

#include <QString>
#include "nwutils.h"
#include "onlinetracker.h"

namespace Mal {

enum UpdateWatchStatus {
    watching = 1,
    completed = 2,
    onhold = 3,
    dropped = 4,
    plantowatch = 6
};

class UpdateItem {
public:
    UpdateItem(const TvShow*);
    static UpdateWatchStatus calculateWatchStatus(const TvShow::WatchStatus status);

    void describe(nw::Describer& de);
    QString toXml();
private:
    int episode;
    UpdateWatchStatus status; // int OR string. 1/watching, 2/completed, 3/onhold, 4/dropped, 6/plantowatch
    short score; // 0 - 10
    int downloaded_episodes;
    int storage_type; // int (will be updated to accomodate strings soon) // yeah sure soon...
    float storage_value; // wat
    int times_rewatched;
    int rewatch_value; // latest rewatched episode
    QDate date_start; // date. mmddyyyy
    QDate date_finish; // date. mmddyyyy
    int priority; // 0 - 10 ? dont know didn't check
    short enable_discussion; // int. 1=enable, 0=disable
    short enable_rewatching; // int. 1=enable, 0=disable
    QString comments; // free text field?
    QString fansub_group;
    QStringList tags; // string. tags separated by commas
};

class Tracker : public OnlineTracker
{
    Q_OBJECT
public:

    class Entry : public OnlineTracker::Entry {
    public:
        Entry(nw::Describer &de);
        int series_animedb_id;
        QString series_title;
        QString series_synonyms;
        int series_type;
        int series_episodes;
        int series_status;
        QDate series_start; //2004-10-05
        QDate series_end;
        QString series_image;
        QString my_id; // always 0 no idea what it does
        int my_watched_episodes;
        QDate my_start_date; // 0000-00-00
        QDate my_finish_date; // 0000-00-00
        int my_score;
        TvShow::WatchStatus my_status;
        int my_rewatching;
        int my_rewatching_ep;
        // field: my_last_updated AS lastUpdated: unix time int example: 1388944557
        QStringList my_tags; // separated by ", "

        void describe(nw::Describer& de);
        void updateShow(const QString trackerIdentifierKey, TvShow* show);
        bool localIsUpToDate(const QString trackerIdentifier, const TvShow* show) const;
        bool remoteIsUpToDate(const TvShow* show) const;
        static TvShow::WatchStatus restoreStatus(int malStatusId);
        bool syncConflict(const QString trackerIdentifier, const TvShow* show) const;
        /// check if data is eq, disregarding the change dates
        bool remoteIsEq(const TvShow* show) const;
    };

    class EntryList : public OnlineTracker::EntryList {
    public:
        EntryList();
        virtual ~EntryList();
        EntryList(nw::Describer& de);

        Entry* get(int remoteId);
        QList<Entry*> items;
        QString error; // should be empty

        void describe(nw::Describer& de);
        void updateShows(const QString trackerIdentifierKey, QList<TvShow*> shows);
        const Entry* get(const QString trackerIdentifierKey, const TvShow* show) const;
    };

    explicit Tracker(OnlineCredentials& credentials, QObject *parent = 0);
    OnlineTracker::UpdateResult updateRemoteImpl(const TvShow* show, const OnlineTracker::EntryList& e) const;
    OnlineTracker::EntryList* fetchRemote() const;
    const QString identifierKey() const;
    static const QString IDENTIFIER_KEY;

signals:

public slots:

private:
    UpdateResult updateinOnlineTrackerOrAdd(const TvShow* show, const QString& type) const;
    CURL* curlTrackerUpdateClient(const char* url, CurlResult& userdata, UpdateItem& data) const;
};


}

#endif // MALTRACKER_H
