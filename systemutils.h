#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QString>

class SystemUtils
{
public:
    SystemUtils();
    static QString fileMime(const QString filepath);
};

#endif // SYSTEMUTILS_H
