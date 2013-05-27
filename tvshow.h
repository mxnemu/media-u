#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <QDir>
#include <season.h>
#include <N0Slib.h>

class TvShow
{
public:
    TvShow(QString name);

    Season &season(QString name);

    void write(QDir& dir) const;

    void importEpisode(const MovieFile& episode);
    QString name() const;


private:
    QString mName;
    QList<Season> seasons;
};

#endif // TVSHOW_H
