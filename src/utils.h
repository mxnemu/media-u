#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>
#include <functional>

class Utils
{
public:
    Utils();

    static int querySimiliarity(const QString &a, const QString &b);
    static QStringList commonSlicesInStrings(const QString a, const QString b);
    static QString commonSliceInStrings(const QStringList& strings);

    static QString createSaveDir(const QString parentDir, const QString dirname);

    template<class T>
    static QList<T> filter(const QList<T> list, typename std::common_type<const std::function<bool(const T&)> >::type filterFn) {
        QList<T> reducedList;
        foreach (T it, list) {
            if (filterFn(it)) {
                reducedList << it;
            }
        }
        return reducedList;
    }

    static int parseRomanNumbers(QString numberString);
    static void removeLastOccurance(QString& baseStr, const QString matchStr);
private:
    static QString commonSliceAtStart(const QString &a, const QString &b, int startIndex);
};

#endif // UTILS_H
