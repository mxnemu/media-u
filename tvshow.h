#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <season.h>

class TvShow
{
public:
    TvShow(QString name);

    Season &season(QString name);


    void importEpisode(const MovieFile& episode);
    QString name() const;


private:
    QString mName;
    QList<Season> seasons;
};

#endif // TVSHOW_H
