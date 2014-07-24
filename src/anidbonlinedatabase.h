#ifndef ANIDBONLINEDATABASE_H
#define ANIDBONLINEDATABASE_H

#include "onlinetvshowdatabase.h"

class AnidbOnlineDatabase : public OnlineTvShowDatabase::Client
{
    Q_OBJECT
public:
    explicit AnidbOnlineDatabase(OnlineCredentials& credentials, QObject *parent);

    class Entry : public OnlineTvShowDatabase::Entry {
        virtual int getRemoteId() const;
        virtual void updateSynopsis(TvShow& show) const;
        virtual void updateTitle(TvShow& show) const;
        virtual void updateRemoteId(TvShow& show) const;
        virtual void updateRelations(TvShow& show) const;
        virtual void updateAiringDates(TvShow& show) const;
        virtual void updateSynonyms(TvShow& show) const;
        virtual void updateImage(TvShow& show, QDir libraryDir) const;

    private:

    };

    class SearchResult : public OnlineTvShowDatabase::SearchResult {
        virtual const Entry* bestEntry() const;
    };

signals:

public slots:

public:
    static const QString IDENTIFIER_KEY;
    virtual const QString identifierKey() { return IDENTIFIER_KEY; }

protected:
    virtual SearchResult* search(QString anime);
};

#endif // ANIDBONLINEDATABASE_H
