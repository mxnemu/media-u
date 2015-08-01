#include "anilistdotcocredentials.h"
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <nwutils.h>

const QString AnilistDotCoCredentials::IDENTIFIER_KEY = "anilist.co";

AnilistDotCoCredentials::AnilistDotCoCredentials(const BaseConfig &config) :
    config(config)
{
    this->userAgent = "nehmu-lfvdn";
    this->secret = "016adWZdNbt1dGf2hLjQL67s";

    this->redirectUri = "http://localhost:8082/api/online/credentials/anilist.co/confirm/";
    this->redirectUri = QString("http://localhost:%1/api/online/credentials/anilist.co/confirm/").arg(config.serverPort());
    nw::JsonReader jr(config.anilistConfigFilePath().toStdString());
    this->token.describeAuthenticate(jr);
    this->updateAuthHeader();

    jr.close();

//    if (!this->token.isValid()) {
//        this->refresh(); // TODO before every curlClient request
//    }
}

const QString AnilistDotCoCredentials::connectUri() const {
    QStringList url = QStringList("https://anilist.co/api/auth/authorize") <<
        QString("?grant_type=authorization_code") <<
        QString("&client_id=%1").arg(userAgent) <<
        QString("&redirect_uri=%1").arg(redirectUri) <<
        QString("&response_type=code");

    return QUrl(url.join("")).toString(QUrl::FullyEncoded);
}

bool AnilistDotCoCredentials::verifyCredentials() {
    if (this->token.isValid()) {
        return true;
    }
    if (!this->token.refreshToken.isEmpty()) {
        return this->refresh();
    }
    return false;
}

// Authorization: Bearer access_token
void AnilistDotCoCredentials::setCredentialsForHandle(CurlResult& userdata, CURL* handle) const {
    QString authString = (QStringList() << "Authorization:"
                                        << "Bearer" //this->token.tokenType
                                        << this->token.accessToken).join(" ");
    struct curl_slist *slist=NULL;
    slist = curl_slist_append(slist, authString.toStdString().c_str());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, slist);

    userdata.slists.append(slist);
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
    QStringList urlSL = QStringList("https://anilist.co/api/auth/access_token") <<
        QString("?grant_type=authorization_code") <<
        QString("&client_id=%1").arg(userAgent) <<
        QString("&client_secret=%1").arg(secret) <<
        QString("&redirect_uri=%1").arg(redirectUri) << // why the heck does this repeat?
        QString("&code=%1").arg(confirmationCode);

    QUrl url(urlSL.join(""));
    CURL* handle = curlClient(url.toString(QUrl::FullyEncoded).toStdString().c_str(), userData);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 0);
    curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, NULL);

    qDebug() << url.toString(QUrl::FullyEncoded);

    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error " << error << " with this message:\n";
        userData.print();
        mHasVerifiedCredentials = false;
    } else {
        nw::JsonReader jr(userData.data);
        token.describeAuthenticate(jr);
        this->updateAuthHeader();
        jr.close();

        if (!token.refreshToken.isEmpty() && token.isValid()) {
            writeToken();
            mHasVerifiedCredentials = true;
        } else {
            mHasVerifiedCredentials = false;
        }
    }

    qDebug() << "anilist.co connection is " << mHasVerifiedCredentials;
    userData.print();
    curl_easy_cleanup(handle);
    return mHasVerifiedCredentials;
}

bool AnilistDotCoCredentials::refresh() {
   /*
        POST: auth/access_token

        Url Parms:
        grant_type    : "refresh_token"
        client_id     :  Client id
        client_secret :  Client secret
        refresh_token :  Refresh Token
    */

    QStringList urlSL = QStringList("https://anilist.co/api/auth/access_token") <<
        QString("?grant_type=refresh_token") <<
        QString("&client_id=%1").arg(userAgent) <<
        QString("&client_secret=%1").arg(secret) <<
        QString("&refresh_token=%1").arg(token.refreshToken);

    QUrl url(urlSL.join(""));
    CurlResult userData(this);
    CURL* handle = curlClient(lock, url.toString(QUrl::FullyEncoded).toStdString().c_str(), userData);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 0);
    curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, NULL);

    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error " << error << " with this message:\n";
        userData.print();
        mHasVerifiedCredentials = false;
    } else {
        nw::JsonReader jr(userData.data);
        this->token.describeRefresh(jr);
        this->updateAuthHeader();
        jr.close();

        this->writeToken();
        if (token.isValid()) {
            mHasVerifiedCredentials = true;
        } else {
            mHasVerifiedCredentials = false;
        }
    }

    qDebug() << "anilist.co connection is " << mHasVerifiedCredentials;
    userData.print();
    curl_easy_cleanup(handle);

    return false;
}

void AnilistDotCoCredentials::writeToken() {
    nw::JsonWriter jw(config.anilistConfigFilePath().toStdString());
    this->token.describeAuthenticate(jw);
    jw.close();
}

void AnilistDotCoCredentials::updateAuthHeader() {
    this->username = token.tokenType;
    this->password = token.accessToken;
}


bool AnilistDotCoCredentials::AuthToken::isValid() {
    return !this->expires.isNull() &&
            this->expires > QDateTime::currentDateTimeUtc();
}

// example
//      { "access_token": "asdfg12345"
//      , "token_type": "bearer" // of the curse, seek authentification.
//      , "expires": 1430858105  // Lest this land expires your token...
//      , "expires_in": 3600     // As it has so many others.
//      , "refresh_token": "qwertzuiop98765432" }
void AnilistDotCoCredentials::AuthToken::describeAuthenticate(nw::Describer &d) {
    this->describeRefresh(d);
    NwUtils::describe(d, "refresh_token", refreshToken);
}

void AnilistDotCoCredentials::AuthToken::describeRefresh(nw::Describer& d) {
    unsigned int unixExpires = expires.toTime_t();
    NwUtils::describe(d, "access_token", accessToken);
    NwUtils::describe(d, "token_type", tokenType);
    NwUtils::describe(d, "expires", unixExpires);
    NwUtils::describe(d, "expires_in", expiresInAsSeconds);
    expires = QDateTime::fromTime_t(unixExpires);
    qDebug() << "oi that better be late" << expires.toTime_t() << QDateTime::currentDateTimeUtc().toTime_t();
}


const QString AnilistDotCoCredentials::identifierKey() const {
    return IDENTIFIER_KEY;
}

