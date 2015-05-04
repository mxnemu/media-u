#ifndef ANILISTDOTCODATABASE_H
#define ANILISTDOTCODATABASE_H

#include "onlinetvshowdatabase.h"

class AnilistDotCoDatabase : public OnlineTvShowDatabase::Client
{
public:

    class Entry {
    };

    class EntryList {
    };

    class SearchResult {
    };

    AnilistDotCoDatabase(OnlineCredentials &credentials, QObject *parent);
    virtual const QString identifierKey() const;

protected:
    virtual OnlineTvShowDatabase::SearchResult* search(QString anime);
};

#endif // ANILISTDOTCODATABASE_H
