#ifndef MOVIEFILE_H
#define MOVIEFILE_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <N0Slib.h>
#include <QDateTime>

class MovieFile : public QObject
{
    Q_OBJECT
public:
    explicit MovieFile(nw::Describer *jw, QObject *parent = 0);
    explicit MovieFile(QString path, QObject *parent = 0);
    
    QString path() const;
    void setPath(QString path);

    void describe(nw::Describer *jw);
    void writeDetailed(nw::JsonWriter& jw);

    static bool hasMovieExtension(QString filename);

    bool isSpecial() const;

    QString episodeName() const;
    QString showName() const;
    QString seasonName() const;
    QString episodeNumber() const;
    QStringList techTags() const;
    QString hashId() const;
    QString releaseGroup() const;

    QString xbmcEpisodeNumber() const;
    QString xbmcEpisodeName() const;
    QString fileExtension() const;
    bool getWatched() const;
    void setWatched(bool value);

    int numericEpisodeNumber() const;
signals:
    void watchedChanged(bool oldValue, bool newValue);
    
public slots:
    
private:
    QString mPath;
    QString mReleaseGroup;
    QString mShowName;
    QString mEpisodeName;
    QStringList mTechTags;
    QString mSeasonName;
    QString mEpisodeNumber;
    QString mHashId;

    QDateTime watchedDate;
};

#endif // MOVIEFILE_H
