#include "anilistdotcodropurl.h"
#include "anilistdotcocredentials.h"

AnilistDotCoDropUrl::AnilistDotCoDropUrl(QObject *parent)
    : OnlineDropUrl(parent)
{
}

bool AnilistDotCoDropUrl::handleUrl(TvShow *show, const QUrl url) {
    if (url.host() != "anilist.co") {
        return false;
    }
    QRegExp imageRegex("^/img/dir/anime/(.+)/([0-9]+)(\\.|-)(.+)?$");
    QRegExp pageRegex("^/anime/([0-9]+)/(.+)$");

    return tryRegex(imageRegex, url, show, 2) ||
           tryRegex(pageRegex,  url, show, 1);
}

bool AnilistDotCoDropUrl::tryRegex(QRegExp regex, const QUrl url, TvShow* show, int cap) {
    int index = url.path().indexOf(regex);
    if (index == -1 || regex.captureCount() < cap) {
        return false;
    }

    bool parseOk = false;
    int remoteId = regex.cap(cap).toInt(&parseOk);
    if (!parseOk) {
        return false;
    }

    show->setRemoteId(AnilistDotCoCredentials::IDENTIFIER_KEY, remoteId);
    return true;
}
