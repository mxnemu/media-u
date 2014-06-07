#ifndef MALCREDENTIALS_H
#define MALCREDENTIALS_H

#include "onlinecredentials.h"

class MalCredentials : public OnlineCredentials
{
public:
    MalCredentials();
    bool verifyCredentials();
    bool hasVerifiedCredentials() const;
    bool readConfig(QString configFilePath);
};

#endif // MALCREDENTIALS_H
