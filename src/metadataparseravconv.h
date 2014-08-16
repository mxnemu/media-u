#ifndef METADATAPARSERAVCONV_H
#define METADATAPARSERAVCONV_H

#include "metadataparser.h"
#include "config.h"

class MetaDataParserAvconv : public MetaDataParser
{
public:
    MetaDataParserAvconv(const AvconvConfig& config);
    virtual MetaData parse(QString filename) const;
    int parseDuration(QString durationString) const;
    QList<MetaDataTrack> parseTracks(QString outputString) const;
    QList<MetaDataChapter> parseChapters(QString outputString) const;

private:
    const AvconvConfig& config;
};

#endif // METADATAPARSERAVCONV_H
