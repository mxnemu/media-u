#ifndef ONLINECREDENTIALS_H
#define ONLINECREDENTIALS_H

#include "curlresult.h"
#include <QString>
#include <QTime>

class OnlineCredentials
{
public:
    OnlineCredentials();
    void set(const QString name, const QString password);
    void set(const QString name, const QString password, QString userAgent);
    virtual bool login() {return mHasVerifiedCredentials || this->verifyCredentials();}

    virtual const QString identifierKey() const = 0;
    virtual bool fetchFirstAuthorizeToken(QString /*code*/) { return false; }
    virtual const QString connectUri() const { return ""; }

    CURL* curlClient(const char* url, CurlResult &userdata);
    CURL* curlNoAuthClient(const char* url, CurlResult& userdata);

    CURL* curlClientNoLock(const char* url, CurlResult &userdata) const;
    CURL* curlNoAuthClientNoLock(const char* url, CurlResult& userdata) const;

    bool readConfig(QString configFilePath);
    bool hasVerifiedCredentials() const;
    QString getUsername() const;


    class TimeLock {
    public:
        TimeLock(int timeToWaitInMs);
        /// puts thread asleep til ready,
        /// returns false if ready was stolen from other thread
        bool blockUntilReady();

    private:
        bool lock();
        QTime timer;
        int timeToWaitInMs;
    };

    TimeLock lock;

protected:
    virtual bool verifyCredentials() = 0;

    bool mHasVerifiedCredentials;
    QString username;
    QString password;
    QString userAgent;
};

#endif // ONLINECREDENTIALS_H
