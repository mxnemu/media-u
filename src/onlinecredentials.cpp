#include "onlinecredentials.h"
#include <QThread>
#include <QFile>
#include "nwutils.h"


OnlineCredentials::OnlineCredentials() :
    lock(1000),
    mHasVerifiedCredentials(false)
{
}

bool OnlineCredentials::readConfig(QString configFilePath) {
    if (!QFile(configFilePath).exists()) {
        return false;
    }
    std::string user, password, userAgent;

    nw::JsonReader jr(configFilePath.toStdString());
    jr.describe("user", user);
    jr.describe("password", password);
    NwUtils::describe(jr, "userAgent", userAgent);
    jr.close();

    if (user.length() > 0 && password.length() > 0) {
        this->set(user.data(), password.data(), userAgent.data());
    }
    return true;
}

CURL* OnlineCredentials::curlNoAuthClientNoLock(const char* url, CurlResult& userdata) const {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, userAgent.toLatin1().data());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

CURL* OnlineCredentials::curlClientNoLock(const char* url, CurlResult& userdata) const {
    CURL* handle = curlNoAuthClientNoLock(url, userdata);
    this->setCredentialsForHandle(userdata, handle);
    return handle;
}

void OnlineCredentials::setCredentialsForHandle(CurlResult& , CURL* handle) const {
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
}

CURL*OnlineCredentials::curlClient(const char* url, CurlResult& userdata) {
    while (this->lock.blockUntilReady()) {
        // sleeps as a side-effect
    }
    return this->curlClientNoLock(url, userdata);
}

CURL*OnlineCredentials::curlNoAuthClient(const char* url, CurlResult& userdata) {
    while (this->lock.blockUntilReady()) {
        // sleeps as a side-effect
    }
    return this->curlNoAuthClientNoLock(url, userdata);
}

void OnlineCredentials::set(const QString name, const QString password) {
    this->username = name;
    this->password = password;
}


void OnlineCredentials::set(const QString name, const QString password, QString userAgent) {
    this->set(name, password);
    this->userAgent = userAgent;
}

bool OnlineCredentials::assureFreshness() {
    if (!this->isFresh()) {
        return this->refresh();
    }
    return true;
}

bool OnlineCredentials::hasVerifiedCredentials() const {
    return mHasVerifiedCredentials;
}

QString OnlineCredentials::getUsername() const
{
    return username;
}



OnlineCredentials::TimeLock::TimeLock(int timeToWaitInMs) :
    timeToWaitInMs(timeToWaitInMs)
{
}

bool OnlineCredentials::TimeLock::blockUntilReady() {
    bool success = this->lock();
    if (!success) {
        QThread::msleep(std::max(0, this->timeToWaitInMs - timer.elapsed()));
        return false;
    }
    return true;
}

bool OnlineCredentials::TimeLock::lock() {
    if (timer.isNull() || this->timer.elapsed() > this->timeToWaitInMs) {
        this->timer.start();
        return true;
    }
    return false;
}
