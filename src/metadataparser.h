#ifndef METADATAPARSER_H
#define METADATAPARSER_H

#include <QStringList>
#include "nwutils.h"

struct VideoTrack {
    int resolutionX;
    int resolutionY;
};

struct AudioTrack {

};

struct SubtitleTrack {

};

enum MetaDataTrackType {
    video, subtitle, attachment, audio
};

struct MetaDataTrack {
    float id;
    QString name;
    union {
        struct VideoTrack video;
        struct AudioTrack audio;
        struct SubtitleTrack subtitle;
    } track;
    int type;
};

struct MetaDataChapter {
    float start;
    float end;
    QString title;
};

class MetaData {
public:
    MetaData();
    int duration;
    QList<MetaDataTrack> tracks;
    QList<MetaDataChapter> chapters;
    QString toJson();
    void describe(nw::Describer *de);
};

class MetaDataParser
{
public:
    MetaDataParser();
    virtual MetaData parse(QString filename) const = 0;
};

#endif // METADATAPARSER_H
