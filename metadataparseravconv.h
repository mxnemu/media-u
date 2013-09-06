#ifndef METADATAPARSERAVCONV_H
#define METADATAPARSERAVCONV_H

#include "metadataparser.h"

class MetaDataParserAvconv : public MetaDataParser
{
public:
    MetaDataParserAvconv();
    virtual MetaData parse(QString filename) const;
    int parseDuration(QString durationString) const;
    QList<MetaDataTrack> parseTracks(QString outputString) const;
    QList<MetaDataChapter> parseChapters(QString outputString) const;
};

#endif // METADATAPARSERAVCONV_H
