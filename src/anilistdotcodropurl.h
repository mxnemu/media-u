#ifndef ANILISTDOTCODROPURL_H
#define ANILISTDOTCODROPURL_H

#include "onlinedropurl.h"

class AnilistDotCoDropUrl : public OnlineDropUrl
{
    Q_OBJECT
    bool tryRegex(QRegExp regex, const QUrl url, TvShow *show, int cap);
public:
    explicit AnilistDotCoDropUrl(QObject *parent = 0);

    virtual bool handleUrl(TvShow* show, const QUrl url);

signals:

public slots:

};

#endif // ANILISTDOTCODROPURL_H
