#ifndef MALCLIENT_H
#define MALCLIENT_H

#include <QObject>
#include <qhttpresponse.h>
#include <qhttprequest.h>
#include <sstream>
#include <QDebug>
#include <curl/curl.h>
#include <QThread>
#include <tvshow.h>
#include "filedownloadthread.h"
#include "nwutils.h"
#include "onlinetvshowdatabase.h"

namespace Mal {
using OnlineTvShowDatabase::Thread;

class Client : public OnlineTvShowDatabase::Client
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void init(QString configFilePath);
    void setCredentials(const QString name, const QString password);
    bool verifyCredentials();
    
    bool hasVerifiedCredentials() const;

    void fetchShows(QList<TvShow *> &showList, QDir libraryDir);
    void fetchShowBlocking(TvShow &show, QDir libraryDir);

    virtual OnlineTvShowDatabase::SearchResult* search(QString anime);
    virtual const OnlineTvShowDatabase::Entry* bestResult(const OnlineTvShowDatabase::SearchResult&result) const;

    Thread* getActiveThread() const;

    bool login();
signals:
    void fetchingFinished();
    
private slots:
    void fetchThreadFinished();

private:
    CURL *curlClient(const char* url, CurlResult &userdata);

    bool mHasVerifiedCredentials;
    QString username;
    QString password;
    QString userAgent;
    Thread* activeThread;
};

class Entry : public OnlineTvShowDatabase::Entry {
public:
    friend class SearchResult;

    int querySimiliarityScore; // TODO not here
    void calculateQuerySimiliarity(const QString query);

    virtual void updateSynopsis(TvShow& show) const;
    virtual void updateTitle(TvShow&) const;
    virtual void updateRemoteId(TvShow& show) const;
    virtual void updateRelations(TvShow&) const;
    virtual void updateAiringDates(TvShow& show) const;
    virtual void updateSynonyms(TvShow& show) const;
    virtual void updateImage(TvShow& show, QDir libraryDir) const;

private:
    Entry(nw::XmlReader& reader);
    void parse(nw::XmlReader& xr);
    void parseSynonyms(nw::XmlReader &reader);

    QString id;
    QString title;
    QString englishTitle;
    QStringList synonyms;
    int episodes;
    QString type;
    QString status;
    QString startDate;
    QString endDate;
    QString synopsis;
    QString image;

    static QString dateFormat;
};

class SearchResult : public OnlineTvShowDatabase::SearchResult {
public:
    SearchResult(CurlResult& result, QString query);
    void parse(CurlResult& result);
    void updateShowFromBestEntry(TvShow& show, QDir libraryDir) const;
    const Entry*bestResult() const;
private:
    QList<Entry> entries;
    QString query;
};

enum UpdateWatchStatus {
    watching = 1,
    completed = 2,
    onhold = 3,
    dropped = 4,
    plantowatch = 6
};

class AnimeUpdateData {
public:
    AnimeUpdateData(TvShow*);
    static UpdateWatchStatus calculateWatchStatus(int watched, int total);
private:
    int episode;
    UpdateWatchStatus status; // int OR string. 1/watching, 2/completed, 3/onhold, 4/dropped, 6/plantowatch
    short score; // 0 - 10
    int downloaded_episodes;
    int storage_type; // int (will be updated to accomodate strings soon) // yeah sure soon...
    float storage_value; // wat
    int times_rewatched;
    short rewatch_value; // 0 - 10 ? dont know didn't check
    QDate date_start; // date. mmddyyyy
    QDate date_finish; // date. mmddyyyy
    int priority; // 0 - 10 ? dont know didn't check
    short enable_discussion; // int. 1=enable, 0=disable
    short enable_rewatching; // int. 1=enable, 0=disable
    QString comments; // free text field?
    QString fansub_group;
    QStringList tags; // string. tags separated by commas
};

} // namespace

#endif // MALCLIENT_H
