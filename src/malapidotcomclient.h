#ifndef MALAPIDOTCOMCLIENT_H
#define MALAPIDOTCOMCLIENT_H

#include <QObject>
#include <QThread>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "onlinetvshowdatabase.h"

namespace MalApiDotCom {
using OnlineTvShowDatabase::UpdateFilter;
using OnlineTvShowDatabase::SearchResult;

class Entry : public OnlineTvShowDatabase::Entry {
public:
    Entry(nw::Describer *de);

    void describe(nw::Describer* de);
    virtual void updateSynopsis(TvShow& show) const;
    virtual void updateTitle(TvShow&) const;
    virtual void updateRemoteId(TvShow& show) const;
    virtual void updateRelations(TvShow& show) const;
    virtual void updateAiringDates(TvShow& show) const;
    virtual void updateSynonyms(TvShow& show) const;
    virtual void updateImage(TvShow& show, QDir libraryDir) const;

    static QString dateFormat;

    int id;

    QString title;
    //other_titles - A hash/dictionary containing other titles this anime has.
    QStringList synonyms;
    QStringList englishTitles;
    QStringList japaneseTitles;

    QString image_url;
    QString type;
    int episodes; // null is returned if the number of episodes is unknown.
    QString status; // Possible values: finished airing, currently airing, not yet aired.
    QString start_date;
    QString end_date;
    QString classification; // This is a freeform text field, with possible values like: R - 17+ (violence & profanity), PG - Children. Not available in /animelist requests.
    QString synopsis;
    QStringList genres;
    QStringList tags; // popular tags set by many users in their personal lists ["supernatural", "comedy"].
    QList<RelatedTvShow> manga_adaptations;
    QList<RelatedTvShow> prequels;
    QList<RelatedTvShow> sequels;
    QList<RelatedTvShow> side_stories;
    RelatedTvShow parent_story;
    QList<RelatedTvShow> character_anime;
    QList<RelatedTvShow> spin_offs;
    QList<RelatedTvShow> summaries;
    QList<RelatedTvShow> alternative_versions;
};

class Client : public OnlineTvShowDatabase::Client {
    Q_OBJECT
public:
    Client(QObject* parent = NULL);

    virtual inline bool login() { return true; }
    virtual SearchResult* search(QString anime);
    virtual const Entry* bestResult(const SearchResult& result) const;
    virtual inline bool updateInOnlineTracker(TvShow*) {return false;}
    virtual inline bool fetchOnlineTrackerList(QList<TvShow*>&) {return false;}


protected:
    SearchResult* parseSearch(CurlResult& response, QString searchedAnime);
    static CURL *curlClient(const char *url, CurlResult &userdata);
};

}

#endif // MALAPIDOTCOMCLIENT_H
