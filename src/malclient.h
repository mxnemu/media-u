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

class Entry : public OnlineTvShowDatabase::Entry {
public:
    friend class SearchResult;

    int querySimiliarityScore; // TODO not here
    void calculateQuerySimiliarity(const QString query);

    virtual int getRemoteId() const;
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
    void updateShowFromBestEntry(TvShow& show, const Library& library) const;
    const Entry*bestResult() const;
private:
    QList<Entry> entries;
    QString query;
};

class Client : public OnlineTvShowDatabase::Client {
    Q_OBJECT
public:
    explicit Client(OnlineCredentials& credentials, QObject *parent = 0);
    void init(QString configFilePath);

    bool hasVerifiedCredentials() const;

    void fetchShows(QList<TvShow *> &showList, const Library& library);
    void fetchShowBlocking(TvShow &show, QDir libraryDir);

    virtual OnlineTvShowDatabase::SearchResult* search(QString anime);
    virtual const OnlineTvShowDatabase::Entry* bestResult(const OnlineTvShowDatabase::SearchResult&result) const;
    virtual const QString identifierKey() const;
    static const QString IDENTIFIER_KEY;
    bool login();

signals:
    void fetchingFinished();

private slots:
    //void fetchThreadFinished();
};

} // namespace

#endif // MALCLIENT_H
