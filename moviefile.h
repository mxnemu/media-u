#ifndef MOVIEFILE_H
#define MOVIEFILE_H

#include <QObject>
#include <QString>
#include <QRegExp>

class MovieFile : public QObject
{
    Q_OBJECT
public:
    explicit MovieFile(QObject *parent = 0);
    
    static bool hasMovieExtension(QString filename);
signals:
    
public slots:
    
};

#endif // MOVIEFILE_H
