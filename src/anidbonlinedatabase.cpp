#include "anidbonlinedatabase.h"

AnidbOnlineDatabase::AnidbOnlineDatabase(OnlineCredentials& credentials, QObject *parent) :
    OnlineTvShowDatabase::Client(credentials, parent)
{
}

AnidbOnlineDatabase::SearchResult*AnidbOnlineDatabase::search(QString anime)
{

}


const QString AnidbOnlineDatabase::IDENTIFIER_KEY = "anidb";


const AnidbOnlineDatabase::Entry*AnidbOnlineDatabase::SearchResult::bestEntry() const
{

}


int AnidbOnlineDatabase::Entry::getRemoteId() const
{

}

void AnidbOnlineDatabase::Entry::updateSynopsis(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateTitle(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateRemoteId(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateRelations(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateAiringDates(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateSynonyms(TvShow& show) const
{

}

void AnidbOnlineDatabase::Entry::updateImage(TvShow& show, QDir libraryDir) const
{

}
