#ifndef ONLINECREDENTIALS_H
#define ONLINECREDENTIALS_H

#include "curlresult.h"
#include <QString>

class OnlineCredentials
{
public:
    OnlineCredentials();
    void set(const QString name, const QString password);
    void set(const QString name, const QString password, QString userAgent);
    virtual bool verifyCredentials() = 0;
    virtual bool login() {return mHasVerifiedCredentials || this->verifyCredentials();}

    CURL* curlClient(const char* url, CurlResult &userdata);
    CURL* curlNoAuthClient(const char* url, CurlResult& userdata);

private:

    bool mHasVerifiedCredentials;
    QString username;
    QString password;
    QString userAgent;
};

#endif // ONLINECREDENTIALS_H
