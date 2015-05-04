#ifndef ANILISTDOTCOTRACKER_H
#define ANILISTDOTCOTRACKER_H

#include "onlinetracker.h"

class AnilistDotCoTracker : public OnlineTracker
{
public:
    AnilistDotCoTracker(OnlineCredentials& credentials, QObject *parent);

    const QString identifierKey();
};

#endif // ANILISTDOTCOTRACKER_H
