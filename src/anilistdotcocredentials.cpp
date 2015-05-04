#include "anilistdotcocredentials.h"
#include <QDebug>
#include <QFile>
#include <nwutils.h>

const QString AnilistDotCoCredentials::IDENTIFIER_KEY = "anilist.co";

AnilistDotCoCredentials::AnilistDotCoCredentials()
{
    this->userAgent = "nehmu-lfvdn";
    this->secret = "016adWZdNbt1dGf2hLjQL67s";
}

const QString AnilistDotCoCredentials::registerUrl() const {
    QStringList url = QStringList("https://anilist.co/api/auth/authorize") <<
        QString("?grant_type=authorization_code") <<
        QString("&client_id=%1").arg(userAgent) <<
        QString("redirect_uri=http://localhost:8082") << // TODO get the actual url/port
        QString("response_type=code");

    return url.join("");
}

bool AnilistDotCoCredentials::verifyCredentials() {
//    if (username.length() <= 0 || password.length() <= 0) {
//        return false;
//    }
    if (this->token.isValid()) {
        return true;
    }
    if (!this->token.refreshToken.isEmpty()) {
        return this->token.refresh();
    }
//    return this->fetchNewToken();
    return false;
}

bool AnilistDotCoCredentials::fetchFirstAuthorizeToken(QString confirmationCode) {
    CurlResult userData(this);
    /*
        POST: auth/access_token

        Url Parms:
        grant_type    : "authorization_code"
        client_id     :  Client id
        client_secret :  Client secret
        redirect_uri  :  Client redirect uri
        code          :  Authorization code
     */
    // Once we have our client authorized, we can
    QStringList url = QStringList("https://anilist.co/api/auth/authorize") <<
        QString("?grant_type=authorization_code") <<
        QString("&client_id=%1").arg(userAgent) <<
        QString("&client_secret=%1").arg(secret) <<
        QString("&redirect_uri=http://localhost:8082") << // why the heck does this repeat?
        QString("&code=%1").arg(confirmationCode);

    CURL* handle = curlClient(url.join("").toStdString().c_str(), userData);

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

    qDebug() << "anilist.co connection is " << mHasVerifiedCredentials;
    userData.print();
    curl_easy_cleanup(handle);
    return mHasVerifiedCredentials;
}

bool AnilistDotCoCredentials::AuthToken::refresh() {
    return false;
}


bool AnilistDotCoCredentials::AuthToken::isValid() {
    return !this->expires.isNull() &&
            this->expires < QDateTime::currentDateTime().addSecs(60); // refresh 60 seconds ahead to compromise for transmission delays
}

AnilistDotCoCredentials::AuthToken AnilistDotCoCredentials::AuthToken::parse(nw::JsonReader r) {
    AnilistDotCoCredentials::AuthToken token;
    NwUtils::describe(r, "access_token", token.accessToken);
    NwUtils::describe(r, "token_type", token.tokenType);
    NwUtils::describe(r, "expires", token.expires);
    NwUtils::describe(r, "expires_in", token.expiresInAsSeconds);
    NwUtils::describe(r, "refresh_token", token.refreshToken);
    return token;
}


const QString AnilistDotCoCredentials::identifierKey() const {
    return IDENTIFIER_KEY;
}

