#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    Utils();

    static int querySimiliarity(const QString &a, const QString &b);
};

#endif // UTILS_H
