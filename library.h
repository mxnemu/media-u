#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QString path, QObject *parent = 0);
    QString randomWallpaperPath() const;
signals:
    
public slots:

private:
    QDir directory;
};

#endif // LIBRARY_H
