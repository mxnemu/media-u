#include "malcredentials.h"
#include <QDebug>
#include <QFile>
#include <nwutils.h>

MalCredentials::MalCredentials()
{
}

bool MalCredentials::readConfig(QString configFilePath) {
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

bool MalCredentials::verifyCredentials() {
    if (username.length() <= 0 || password.length() <= 0) {
        return false;
    }

    CurlResult userData(this);
    CURL* handle = curlClient("http://myanimelist.net/api/account/verify_credentials.xml", userData);
    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error " << error << " with this message:\n";
        userData.print();
    } else {
        if (userData.data.str() == "Invalid credentials") {
            mHasVerifiedCredentials = false;
        } else {
            mHasVerifiedCredentials = true;
        }
    }

    qDebug() << "mal connection is " << mHasVerifiedCredentials;
    curl_easy_cleanup(handle);
    return mHasVerifiedCredentials;
}
