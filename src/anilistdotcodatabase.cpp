#include "anilistdotcodatabase.h"
#include "anilistdotcocredentials.h"

AnilistDotCoDatabase::AnilistDotCoDatabase(OnlineCredentials& credentials, QObject* parent = NULL) :
    OnlineTvShowDatabase::Client(credentials, parent)
{

}

const QString AnilistDotCoDatabase::identifierKey() const {
    return AnilistDotCoCredentials::IDENTIFIER_KEY;
}

OnlineTvShowDatabase::SearchResult* AnilistDotCoDatabase::search(QString anime) {
    return NULL;
}
