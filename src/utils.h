#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>

class Utils
{
public:
    Utils();

    static int querySimiliarity(const QString &a, const QString &b);
    static QStringList commonSlicesInStrings(const QString a, const QString b);
    static QString commonSliceInStrings(const QStringList& strings);

private:
    static QString commonSliceAtStart(const QString &a, const QString &b, int startIndex);
};

#endif // UTILS_H
