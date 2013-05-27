#include "moviefile.h"
#include <QFile>
#include <iostream>

MovieFile::MovieFile(QString path, QObject *parent) :
    QObject(parent)
{
    setPath(path);
    watched = false;
}

QString MovieFile::path() const {
    return mPath;
}

void MovieFile::setPath(QString path) {
    mPath = path;
    path = QFile(path).fileName();

    if (!path.contains(' ')) {
        path.replace('_', ' ');
        path.replace('.', ' ');
    }

    QRegExp regexGroup("\\[(.*)\\]");
    regexGroup.setMinimal(true);
    int groupIndex = regexGroup.indexIn(path);
    mReleaseGroup = regexGroup.cap(1);

    // TODO differ techtags in [] from release groups
    // hints: at the end, multiple [] like [720p][AAC]
    // comma separated [720p, AAC]
    // space separated [720p AAC]
    // TODO continue coding here

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    if (groupIndex >= -1 && groupIndex <= 1) {
        QRegExp regexName("(.*)(( -)|\\[|\\(|(OP[0-9])|(ED[0-9])|(EP[0-9])|(SP[0-9])|(Episode\s?[0-9])|$)");
        regexName.setMinimal(true);

        int groupLength = mReleaseGroup.length() + 2;
        groupLength = groupLength == 2 ? 0 : groupLength;
        int nameIndex = regexName.indexIn(path.right(path.length() - groupLength));
        mName = regexName.cap(1).trimmed();

        path.remove(nameIndex, regexName.cap(1).length());

    // showname[Group] - 01v2 (techtags)[12345ABC].webm
    } else if (groupIndex > 0) {
        QRegExp regexName("^(.+)\\[");

    // [Group] showname - 01v2 (techtags)[12345ABC].webm    
    } else {
        QRegExp regexName("^(.+)\\[");
    }

    QRegExp regexEpisode("(\\s[0-9]+(v[0-9]+)?\\s|ED[0-9]+|OP[0-9]+[a-z]?|SP[0-9]+|EP[0-9]+|Episode\\s?[0-9]+)", Qt::CaseInsensitive);
    //regexEpisode.setMinimal(true);
    int episodeIndex = regexEpisode.indexIn(path);
    mEpisodeNumber = regexEpisode.cap(1).trimmed();

    path.remove(episodeIndex, regexEpisode.cap(1).length());

    QRegExp regexSeason("((SE?)[0-9]+|(Season\\s?)[0-9]+)", Qt::CaseInsensitive);
    regexSeason.setMinimal(true);
    int seasonIndex = regexSeason.indexIn(path);
    if (seasonIndex != -1) {
        mSeasonName = regexSeason.cap(1);
        path.remove(seasonIndex, regexSeason.cap(1).length());
    }

}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv|\\.ogv|\\.mpeg|\\.mp4|\\.webm|\\.avi", Qt::CaseInsensitive));
}

QString MovieFile::releaseGroup() const {
    return mReleaseGroup;
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
