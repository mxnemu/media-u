#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <N0Slib.h>
#include "moviefile.h"

class Season
{
public:
    Season(QString name);
    virtual ~Season();

    void writeAsElement(nw::JsonWriter& jw) const;

    void addEpisode(const MovieFile& file);
    QString name() const;

private:
    QList<MovieFile*> episodes;
    QString mName;
};

#endif // SEASON_H
