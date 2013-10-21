#ifndef FRANCHISE_H
#define FRANCHISE_H

#include <QObject>
#include "tvshow.h"

class Franchise : public QObject
{
    Q_OBJECT
public:
    explicit Franchise(QObject *parent = 0);
    
    bool hasRelationTo(const TvShow* show) const;
    void addTvShow(const TvShow *show);
    void generateName();
signals:
    void nameGenerated();
public slots:

private:
    QList<const TvShow*> tvShows;
    QString name;
};

#endif // FRANCHISE_H
