#ifndef MOVIEFILEA_H
#define MOVIEFILEA_H

#include <QString>
#include "nwutils.h"

class MovieFile {
public:
    MovieFile(const QString originalPath);

    static bool hasMovieExtension(QString filename);

    void writeForApi(nw::Writer& de) const;
    bool isSpecial() const;
    static bool isSpecial(QString episodeNumberString);
    float numericEpisodeNumber() const;

    QString xbmcEpisodeNumber() const;
    QString xbmcEpisodeName() const;
    QString fileExtension() const;
    bool exists() const;

    // keep public and only create const instances of this
    QString path;
    QString releaseGroup;
    QString episodeName;
    QStringList techTags;
    QString showName;
    QString seasonName;
    QString episodeNumber;
    QString hashId;

    static const int UNKNOWN = -1;
    static const int SPECIAL = -2;
    static const int INVALID = -3;
};

#endif // MOVIEFILE_H
