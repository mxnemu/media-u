#ifndef MALAPIDOTCOMCLIENT_H
#define MALAPIDOTCOMCLIENT_H

#include <QObject>
#include <QThread>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"

namespace MalApiDotCom {

class Entry {
public:
    Entry(nw::Describer* de);
    void describe(nw::Describer* de);
    void updateShow(TvShow& show, QDir &libraryDir) const;

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

class SearchResult {
public:
    SearchResult();
    SearchResult(CurlResult& response, QString searchedAnime);


    void describe(nw::Describer* const de);
    const Entry *bestResult();
    QString searchedAnime;
    QList<Entry> entries;
};

class Client : public QObject {
    Q_OBJECT
public:
    Client();

    bool updateShow(TvShow& show, QDir &libraryDir);
    SearchResult search(QString anime);
private:
    static CURL *curlClient(const char *url, CurlResult &userdata);
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(Client &client, QList<TvShow*> &shows, QDir libraryDir, QObject *parent);

    void run();

private:
    Client &client;
    QList<TvShow*> &tvShows;
    QDir libraryDir;
};

}

#endif // MALAPIDOTCOMCLIENT_H
