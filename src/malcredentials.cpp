#include "malcredentials.h"

MalCredentials::MalCredentials()
{
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

bool MalCredentials::hasVerifiedCredentials() const {
    return mHasVerifiedCredentials;
}

bool MalCredentials::login() {
    return this->hasVerifiedCredentials() || this->verifyCredentials();
}
