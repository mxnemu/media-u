#include "moviefile.h"
#include <QFileInfo>
#include <iostream>
#include <stdlib.h>
#include <QDebug>
#include "nwutils.h"

MovieFile::MovieFile(nw::Describer *jw, QObject* parent) :
    QObject(parent)
{
    this->describe(jw);
}

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
    path = QFileInfo(path).completeBaseName();


    int spaces = path.count(' ');
    if (path.count('_') > spaces) {
        path.replace('_', ' ');
    }
    if (path.count('.') > spaces) {
        path.replace('.', ' ');
    }

    /*
    // TODO match only from latest dot. Now-workaround: match after . replace
    QRegExp extensionRegex("(\\..+$)");
    int extensionIndex = extensionRegex.indexIn(path);
    path.remove(extensionIndex, extensionRegex.cap(1).length());
    */

    int groupIndex = -2;
    while (groupIndex != -1) {
        QRegExp regexGroup("^(\\[.*\\])");
        regexGroup.setMinimal(true);
        groupIndex = regexGroup.indexIn(path);
        if (groupIndex != -1) {
            mReleaseGroup.append(regexGroup.cap(1));
            path.remove(0, regexGroup.cap(1).length());
        }
    }

    QRegExp regexHashId("(\\[[A-F0-9]{8}\\])");
    int hashIdIndex = regexHashId.indexIn(path);
    if (hashIdIndex != -1) {
        mHashId = regexHashId.cap(1);
        path.remove(hashIdIndex, regexHashId.cap(1).length());
    }


    QRegExp techTags("((\\[.*\\])|(\\(.*\\)))");
    techTags.setMinimal(true);
    int techTagsIndex = techTags.indexIn(path);
    while (techTagsIndex != -1) {
        path.remove(techTagsIndex, techTags.cap(1).length());
        techTagsIndex = techTags.indexIn(path);
    }

    // TODO differ techtags in [] from release groups
    // hints: at the end, multiple [] like [720p][AAC]
    // comma separated [720p, AAC]
    // space separated [720p AAC]

    // [Group] showname - 01v2 (techtags)[12345ABC].webm
    if (groupIndex >= -1 && groupIndex <= 1) {
        QRegExp regexName("(.*)(( -)|\\[|\\(|(OP[0-9])|(ED[0-9])|(EP[0-9])|(SP[0-9])|(Episode\\s?[0-9])|$)");
        regexName.setMinimal(true);
        int nameIndex = regexName.indexIn(path);
        mShowName = regexName.cap(1).trimmed();

        if (nameIndex != -1 || mShowName.length() <= 0) {
            path.remove(nameIndex, regexName.cap(1).length());
        } else {
            qDebug() << "could not parse name out of " << path;
        }

    // showname[Group] - 01v2 (techtags)[12345ABC].webm
    } else if (groupIndex > 0) {
        QRegExp regexName("^(.+)\\[");

    // [Group] showname - 01v2 (techtags)[12345ABC].webm    
    } else {
        QRegExp regexName("^(.+)\\[");
    }

    QRegExp regexEpisode("("
        "\\s[0-9]+((v|\\.)[0-9]+)?(\\s|$)|"
        "\\s[0-9]+(\\[v[0-9]+\\])(\\s|$)|"
        "[0-9]+x[0-9]+|"
        "\\sED(\\s|$)|"
        "\\sOP(\\s|$)|"
        "ED[0-9]+|"
        "OP[0-9]+[a-z]?|"
        "SP[0-9]+|"
        "EP[0-9]+|"
        "NC.?OP([0-9]+)?|"
        "NC.?ED([0-9]+)?|"
        "EX([0-9]+)?|"
        "Episode\\s?[0-9]+|"
        "Opening(\\s?[0-9]+)?|"
        "Ending(\\s?[0-9]+)?"
        ")", Qt::CaseInsensitive);
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

    // TODO check for shows with a [0-9]+ ending and just 1 episode to avoid some false positives
    if (mEpisodeNumber.isEmpty() || mEpisodeNumber.isNull()) {
        //QRegExp regexNumberAfterShowName("(\\s[0-9]+\\s?)$");
        //int epIndex = regexNumberAfterShowName.indexIn(mShowName);
        int epIndex = regexEpisode.indexIn(mShowName);
        // TODO check all caps for the longest
        if (epIndex != -1) {
            mEpisodeNumber = regexEpisode.cap(1).trimmed();
            mShowName.remove(epIndex, regexEpisode.cap(1).length());

            QRegExp regexEpName("(.*)");
            int epNameIndex = regexEpName.indexIn(mShowName, epIndex);
            if (epNameIndex != -1) {
                mEpisodeName = regexEpName.cap(1).trimmed();
                mShowName.remove(epIndex, regexEpName.cap(1).length());
            }
            mShowName = mShowName.trimmed();
        }
    }
}

void MovieFile::describe(nw::Describer *jw) {
    NwUtils::describe(*jw, "path", mPath);
    jw->describe("watched", watched);
}

void MovieFile::writeDetailed(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "episodeNumber", mEpisodeNumber);
    NwUtils::describe(jw, "path", mPath);
    NwUtils::describe(jw, "releaseGroup", mReleaseGroup);
    NwUtils::describe(jw, "showName", mShowName);
    NwUtils::describe(jw, "episodeName", mEpisodeName);
    //NwUtils::describe(jw, "tech", mTechTags);
    NwUtils::describe(jw, "seasonName", mSeasonName);
    NwUtils::describe(jw, "hashId", mHashId);
}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv$|\\.ogv$|\\.mpeg$|\\.mp4$|\\.webm$|\\.avi$", Qt::CaseInsensitive));
}

// TODO specialsRegex into a separate function to avoid redundance
bool MovieFile::isSpecial() const {
    QRegExp specialRegex("("
        "\\sED(\\s|$)|"
        "\\sOP(\\s|$)|"
        "ED[0-9]+|"
        "OP[0-9]+[a-z]?|"
        "SP[0-9]+|"
        "EP[0-9]+|"
        "NC.?OP([0-9]+)?|"
        "NC.?ED([0-9]+)?|"
        "Opening(\\s?[0-9]+)?|"
        "Ending(\\s?[0-9]+)?"
        ")", Qt::CaseInsensitive);
    bool is = -1 != specialRegex.indexIn(this->episodeName());
    //qDebug() << specialRegex.cap(1);
    return is;
}

QString MovieFile::releaseGroup() const {
    return mReleaseGroup;
}

QString MovieFile::xbmcEpisodeNumber() const {
    if (episodeNumber().contains(QRegExp("OP|ED|Opening|Ending|EX"))) {
        return QString("0x%1").arg(episodeNumber());
    }
    QRegExp pureNumber("([0-9]+)");
    int index = pureNumber.indexIn(mEpisodeNumber);
    if (index != -1) {
        return pureNumber.cap(1);
    }
    return episodeNumber();
}

QString MovieFile::fileExtension() const {
    return QFileInfo(mPath).completeSuffix();
}

QString MovieFile::xbmcEpisodeName() const {
    if (episodeName().length() > 0) {
        return episodeName();
    }
    return "Episode";
}

QString MovieFile::episodeName() const {
    return mEpisodeName;
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
