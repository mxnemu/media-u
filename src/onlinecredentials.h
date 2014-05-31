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
    virtual bool verifyCredentials() = 0;
    virtual bool login() {return mHasVerifiedCredentials || this->verifyCredentials();}

    CURL* curlClient(const char* url, CurlResult &userdata);
    CURL* curlNoAuthClient(const char* url, CurlResult& userdata);

    CURL* curlClientNoLock(const char* url, CurlResult &userdata) const;
    CURL* curlNoAuthClientNoLock(const char* url, CurlResult& userdata) const;

    bool hasVerifiedCredentials() const;
    QString getUsername() const;

protected:
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

    bool mHasVerifiedCredentials;
    QString username;
    QString password;
    QString userAgent;
    TimeLock lock;
};

#endif // ONLINECREDENTIALS_H
