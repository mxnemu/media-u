#ifndef TVSHOW_H
#define TVSHOW_H

#include <QList>
#include <QDate>
#include <QDir>
#include <season.h>
#include <N0Slib.h>

class TvShow
{
public:
    TvShow(QString name);

    Season &season(QString name);

    void read(QDir &dir);
    void write(QDir& dir);

    void importEpisode(const MovieFile& episode);
    void downloadImage(const QString url);

    bool isAiring() const;

    QString name() const;
    QStringList getSynonyms() const;
    QString getAiringStatus() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    int getTotalEpisodes() const;
    QString getSynopsis() const;
    QString getShowType() const;
    QString getRemoteId() const;

    void setSynonyms(const QStringList &value);
    void setAiringStatus(const QString &value);
    void setStartDate(const QDate &value);
    void setEndDate(const QDate &value);
    void setTotalEpisodes(int value);
    void setShowType(const QString &value);
    void setSynopsis(const QString &value);
    void setRemoteId(const QString &value);

private:
    QDir directory;
    QString mName;
    QList<Season> seasons;

    QString remoteId;
    QStringList synonyms;
    QString showType;
    QString airingStatus;
    QDate startDate;
    QDate endDate;
    int totalEpisodes;
    QString synopsis;
};

#endif // TVSHOW_H
