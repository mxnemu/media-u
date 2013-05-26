#ifndef MOVIEFILE_H
#define MOVIEFILE_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QStringList>

class MovieFile : public QObject
{
    Q_OBJECT
public:
    explicit MovieFile(QString path, QObject *parent = 0);
    
    QString path() const;
    void setPath(QString path);

    static bool hasMovieExtension(QString filename);

    QString name() const;
    QString showName() const;
    QString seasonName() const;
    QString episodeNumber() const;
    QStringList techTags() const;
    QString hashId() const;
    QString releaseGroup() const;
signals:
    
public slots:
    
private:
    QString mPath;
    QString mReleaseGroup;
    QString mShowName;
    QString mName;
    QStringList mTechTags;
    QString mSeasonName;
    QString mEpisodeNumber;
    QString mHashId;

    bool watched;
};

#endif // MOVIEFILE_H
