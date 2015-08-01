#ifndef MOVIESCANNER_H
#define MOVIESCANNER_H

#include <QString>
#include <QThread>
#include "searchdirectory.h"
#include "mediascanner.h"

class DirectoryScanner : public QObject
{
    Q_OBJECT
public:
    DirectoryScanner();
    virtual ~DirectoryScanner();
    void addScanner(MediaScanner* scanner);

    void scan(QString path);
    void scan(const QString& path, QStringList& handledDirs, const QString& rootPath);
    void scanFiles(const QStringList &files);
signals:
    void machingFile(QString);
private:

    QList<MediaScanner*> mediaScanners;
};


class DirectoryScannerThread : public QThread {
    Q_OBJECT
public:
    DirectoryScannerThread(DirectoryScanner* scanner, const QList<SearchDirectory> dirs, QObject* parent);
    virtual ~DirectoryScannerThread();
    void run();
signals:
    void machingFile(QString);
    void done();
private:
    DirectoryScanner* scanner;
    const QList<SearchDirectory> dirs;
};

#endif // MOVIESCANNER_H
