#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <N0Slib.h>
#include <QDir>
#include "moviefile.h"

class Season
{
public:
    Season(QString name);
    virtual ~Season();

    void exportXbmcLinks(QDir dir);
    void writeAsElement(nw::JsonWriter& jw);
    void readAsElement(nw::JsonReader &jr);

    void addEpisode(const MovieFile& file);
    void addEpisode(QString file);
    QString name() const;
    int numberOfEpisodes() const;

private:
    QList<MovieFile*> episodes;
    QString mName;
};

#endif // SEASON_H
