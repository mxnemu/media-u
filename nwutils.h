#ifndef NW_UTILS_H
#define NW_UTILS_H

#include <N0Slib.h>
#include <QDate>
#include <QString>

class NwUtils
{
public:
    NwUtils();

    template<class T> static void describe(T& d, const nw::String key, QDate& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toString(dateFormat).toStdString();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QDate::fromString(QString(str.data()), dateFormat);
        }
    }

    template<class T> static void describe(T& d, const nw::String key, QTime& value) {}
    template<class T> static void describe(T& d, const nw::String key, QDateTime& value) {}
    template<class T> static void describe(T& d, const nw::String key, QString& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toStdString();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QString(str.data());
        }
    }

    template<class T, class T2> inline static void describe(T& d, const nw::String key, T2& value) {
        d.describe(key, value);
    }

    static const QString dateFormat;
};

#endif // NW_UTILS_H
