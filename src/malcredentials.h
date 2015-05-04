#ifndef MALCREDENTIALS_H
#define MALCREDENTIALS_H

#include "onlinecredentials.h"

class MalCredentials : public OnlineCredentials
{
public:
    MalCredentials();
    bool verifyCredentials();
    bool hasVerifiedCredentials() const;
    virtual const QString identifierKey() const;
};

#endif // MALCREDENTIALS_H
