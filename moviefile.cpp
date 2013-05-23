#include "moviefile.h"

MovieFile::MovieFile(QString path, QObject *parent) :
    QObject(parent)
{
    setPath(path);
}

QString MovieFile::path() const {
    return mPath;
}

void MovieFile::setPath(QString path) {
    mPath = path;

    QRegExp regexGroup("\\[(.+)\\]");
    int groupIndex = regexGroup.indexIn(path);
    mReleaseGroup = regexGroup.cap(1);

    // TODO differ techtags in [] from release groups
    // hints: at the end, multiple [] like [720p][AAC]
    // comma separated [720p, AAC]
    // space separated [720p AAC]

    // TODO continue coding here

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    if (groupIndex == 0) {
        QRegExp regexName("");

    // showname[Group] - 01v2 (techtags)[12345ABC].webm
    } else if (groupIndex > 0) {
        QRegExp regexName("^(.+)\\[");

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    } else {
        QRegExp regexName("^(.+)\\[");
    }

}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv|\\.ogv|\\.mpeg|\\.mp4|\\.webm|\\.avi", Qt::CaseInsensitive));
}

QString MovieFile::name() const {
    return mName;
}

QString MovieFile::showName() const {
    return mShowName;
}

QString MovieFile::seasonName() const {
    return mSeasonName;
}

QString MovieFile::episodeNumber() const {
    return mEpisodeNumber;
}

QStringList MovieFile::techTags() const {
    return mTechTags;
}
QString MovieFile::hashId() const {
    return mHashId;
}
