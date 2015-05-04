#ifndef ANILISTDOTCOCREDENTIALS_H
#define ANILISTDOTCOCREDENTIALS_H

#include "onlinecredentials.h"
#include "nwutils.h"

class AnilistDotCoCredentials : public OnlineCredentials
{
public:
    class AuthToken {
    public:
        /* Example:
        {
          access_token: "ACXD3snrImEP5R6IHs6gGgqpnGgoZp54TDaWZkgc"
          token_type: "bearer"
          expires: 1414232110
          expires_in: 3600
          refresh_token: "X2Bxj1KzjsoaD4FCj6A0MGFWdYlGgoc31L70eSAQ"
        }
        */
        QString accessToken;
        QString tokenType; // I don't know what to do with this, yet
        QDateTime expires;
        int expiresInAsSeconds;
        QString refreshToken;

        bool isValid();
        bool refresh();

        static AuthToken parse(nw::JsonReader reader);
    };

    AnilistDotCoCredentials();

    //////////////////////////////////////
    /// There are 3 Tokens/steps: ("never used the API before" / "using the API right now" / "paused using")
    /// 1) "code" (received from /authorize redirecting to localhost)
    /// 2) "authorization_code" (received from giving the "code" and api-secret to anilist.co)
    ///         You need to send this to authorize every API request you make.
    ///         It runs out after 1 hour, go to step 3.
    /// 3) "refresh_code" comes with the first "authorization_code".
    ///         you must send it every hour to get a new ""authorization_code".
    ///         this brings you back to step 2.
    ///         You should store it to avoid going to step 1 again.
    /////////////////////////////////////////
    /// The first step happens outside in the web-ui.
    /// We must provide a href for the web-ui that takes the user to
    /// anilist.co and asks them to accept our client.
    /// We set /api/online-confirmation/anilist.co/ as redirect-uri to
    /// witch anilist.co brings us back and define an API that calls the next method.
    /////////////////////////////////////////
    const QString registerUrl() const; // this should for once not have any side effects
    /////////////////////////////////////////
    /// This method is called by our localhost:8082 server, when anilist.co
    /// redirects the user with the confirmation "code".
    /// We request the actual auth token and our refresh token for the future.
    /////////////////////////////////////////
    virtual bool fetchFirstAuthorizeToken(QString code);
    /////////////////////////////////////////
    /// Once the client is already registered and our first auth token has run out
    /// We must call this method to request a fresh auth token using our stored refresh_key
    /////////////////////////////////////////
    bool refreshAuthorizationToken();

    bool login() { return verifyCredentials(); }
    bool verifyCredentials();

    virtual const QString identifierKey() const;
    static const QString IDENTIFIER_KEY;

private:
    AuthToken token;
    QString secret;
};

#endif // ANILISTDOTCOCREDENTIALS_H
