#ifndef MOVIEFILEA_H
#define MOVIEFILEA_H

#include <QString>
#include "nwutils.h"

class MovieFile {
public:
    MovieFile(QString path);

    static bool hasMovieExtension(QString filename);

    bool isSpecial() const;
    int numericEpisodeNumber() const;

    QString xbmcEpisodeNumber() const;
    QString xbmcEpisodeName() const;
    QString fileExtension() const;

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
