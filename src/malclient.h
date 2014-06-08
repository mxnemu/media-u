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
    virtual const OnlineTvShowDatabase::Entry* bestEntry() const;
private:
    QList<Entry> entries;
};

class Client : public OnlineTvShowDatabase::Client {
    Q_OBJECT
public:
    explicit Client(OnlineCredentials& credentials, QObject *parent = 0);
    void init(QString configFilePath);

    virtual const QString identifierKey() const;
    static const QString IDENTIFIER_KEY;

protected:
    virtual OnlineTvShowDatabase::SearchResult* search(QString anime);
};

} // namespace

#endif // MALCLIENT_H
