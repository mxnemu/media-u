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

    static bool commandExists(QString command);
private:
    static void removeNewline(char *buffer);
};

#endif // SYSTEMUTILS_H
