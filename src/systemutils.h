#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QString>
#include <QProcess>

class SystemUtils
{
public:
    SystemUtils();
    static QString fileMime(const QString filepath);
    static int setProcessPriority(QProcess& process, int nice);
};

#endif // SYSTEMUTILS_H
