#ifndef ONLINEDROPURL_H
#define ONLINEDROPURL_H

#include <QObject>
#include <tvshow.h>
#include <QUrl>

/// This class will be contacted when the user drag'n drops
///     a "http://" url onto the remote controller, or the qt window.
/// It's purpose is to use the url to modify a tv show, by
///     setting a wallpaper / remote id / other infos contained in the url
class OnlineDropUrl : public QObject
{
    Q_OBJECT
public:
    explicit OnlineDropUrl(QObject *parent = 0);

    virtual bool handleUrl(TvShow* show, const QUrl url) = 0;

signals:

public slots:

};

#endif // ONLINEDROPURL_H
