#ifndef MOVIEFILEA_H
#define MOVIEFILEA_H

#include <QString>
#include "nwutils.h"

/// This parses useful info out of a VideoFileName
/// I made all data public for easy access, so please only create const instances.
/// Side-effects:
/// It might check the parent directory name & symlinks
class VideoFile {
public:
    VideoFile(const QString originalPath);

    static bool hasVideoExtension(QString filename);

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
