#ifndef ANIDBCREDENTIALS_H
#define ANIDBCREDENTIALS_H

#include "onlinecredentials.h"

class AnidbCredentials : public OnlineCredentials
{
public:
    explicit AnidbCredentials();

    class Response {

    };



signals:

public slots:

protected:
    virtual bool verifyCredentials();

private:
    static const int protocolVersion;
};

#endif // ANIDBCREDENTIALS_H
