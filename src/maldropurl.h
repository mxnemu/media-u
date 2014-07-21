#ifndef MALDROPURL_H
#define MALDROPURL_H

#include <onlinedropurl.h>

class MalDropUrl : public OnlineDropUrl
{
    Q_OBJECT
public:
    explicit MalDropUrl(QObject *parent = 0);

    virtual bool handleUrl(TvShow* show, const QUrl url);

signals:

public slots:

};

#endif // MALDROPURL_H
