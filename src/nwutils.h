#ifndef NW_UTILS_H
#define NW_UTILS_H

#include <N0Slib.h>
#include <QDate>
#include <QString>
#include <QStringList>

class NwUtils
{
public:
    NwUtils();

    template<class T> static void describe(T& d, const nw::String key, QDate& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toString(Qt::ISODate).toStdString();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QDate::fromString(QString(str.data()), Qt::ISODate);
        }
    }

    template<class T> static void describe(T& d, const nw::String key, QTime& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toString(Qt::ISODate).toStdString();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QTime::fromString(QString(str.data()), Qt::ISODate);
        }
    }

    template<class T> static void describe(T& d, const nw::String key, QDateTime& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toString(Qt::ISODate).toStdString();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QDateTime::fromString(QString(str.data()), Qt::ISODate);
        }
    }
    template<class T> static void describe(T& d, const nw::String key, QString& value) {
        d.describeName(key);
        describeValue(d, value);
    }

    template<class T> static void describeValue(T& d, QString& value) {
        if (d.isInWriteMode()) {
            std::string str = value.toStdString();
            d.describeValue(str);
        } else {
            std::string str;
            d.describeValue(str);
            value = QString(str.data());
        }
    }

    template<class T> static void describe(T& d, const nw::String key, QStringList& value, char separator) {
        if (d.isInWriteMode()) {
            std::stringstream ss;
            for (int i=0; i < value.length(); ++i) {
                if (i != 0) {
                    ss << separator;
                }
                ss << value.at(i).toStdString();
            }
            std::string str = ss.str();
            d.describe(key, str);
        } else {
            std::string str;
            d.describe(key, str);
            value = QString(str.data()).split(separator);
        }
    }

    template<class T> static void describeValueArray(T& d, const nw::String arrayName, QStringList& value) {
        if (d.isInReadMode()) {
            value.clear();
        }

        d.describeValueArray(arrayName, value.length());
        for (int i=0; d.enterNextElement(i); ++i) {
            nw::String str;
            if (d.isInWriteMode()) {
                str = value[i].toStdString();
            }
            d.describeValue(str);
            if (d.isInReadMode()) {
                value.append(str.data());
            }
        }
    }

    template<class T, class T2> inline static void describe(T& d, const nw::String key, T2& value) {
        d.describe(key, value);
    }
};

#endif // NW_UTILS_H
