#include "utils.h"
#include <QStringList>
#include <QDir>
#include <QDebug>

Utils::Utils()
{
}

int Utils::querySimiliarity(const QString& a, const QString& b) {
    QRegExp wordSplitRegex("(\\b)?.+?(\\s|\\b|$)");
    wordSplitRegex.indexIn(a);
    QStringList aWords = a.split(" ");
    wordSplitRegex.indexIn(b);
    QStringList bWords = b.split(" ");

    int matches = 0;
    for (int i=0; i < aWords.length(); ++i) {
        const QString& aWord = aWords.at(i).trimmed();
        for (int j=0; j < bWords.length(); ++j) {
            const QString bWord = bWords.at(j).trimmed();
            if (bWord == aWord) {
                matches += 1;
                break;
            }
        }
    }
    return matches;
}

QStringList Utils::commonSlicesInStrings(const QString aParm, const QString bParm) {
    // sort by length
    const QString& a = aParm.length() >= bParm.length() ? aParm : bParm;
    const QString& b = &a == &aParm ? bParm : aParm;

    QString result;
    QStringList results;
    int index = -1;
    do {
        //index = index + result.length(); // the fast & sloppy way
        index = index + 1;
        result = commonSliceAtStart(a,b, index);
        if (result.length() > 1) {
            results << result;
        }
    } while (index < a.length());
    return results;
}

QString Utils::commonSliceAtStart(const QString& a, const QString& b, int startIndex) {
    QString testCommon;
    QString lastCommon;
    for (int i=startIndex; i < a.length(); ++i) {
        testCommon.append(a.at(i));
        if (-1 != b.indexOf(testCommon, 0, Qt::CaseInsensitive)) {
            lastCommon = testCommon;
        } else {
            break;
        }
    }
    return lastCommon.trimmed();
}

QString Utils::commonSliceInStrings(const QStringList &strings) {
    QStringList commonResults;
    for (int i=0; i < strings.length(); ++i) {
        QStringList results;
        bool testedForResults = false;
        for (int j=i+1; j < strings.length(); ++j) {
            const QString& a = strings.at(i);
            const QString& b = strings.at(j);
            QStringList newResults = commonSlicesInStrings(a, b);
            foreach (QString r, newResults) {
                if (!results.contains(r)) {
                    results << r;
                }
            }

            testedForResults = true;
        }
        if (0 == i) {
            commonResults = results;
        } else if (testedForResults) {
            for (int k=0; k < commonResults.length(); ++k) {
                bool found = false;
                foreach (QString r, results) {
                    if (0 == r.compare(commonResults.at(k), Qt::CaseInsensitive)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    commonResults.removeAt(k);
                    --k;
                }
            }
        }
    }

    QString empty = QString();
    QString& bestResult = empty;
    foreach (const QString& r, commonResults) {
        if (r.length() > bestResult.length()) {
            bestResult = r;
        }
    }

    return bestResult;
}

int Utils::parseRomanNumbers(QString numberString) {
    static const std::map<QString, int> signs = {
        std::make_pair("I", 1),
        std::make_pair("V", 5),
        std::make_pair("X", 10),
        std::make_pair("L", 50),
        std::make_pair("C", 100),
        std::make_pair("D", 500),
        std::make_pair("M", 1000),
        std::make_pair("ↁ", 50000),
        std::make_pair("ↂ", 10000)
    };

    int totalSum = 0;
    int currentSum = 0;
    int lastSignValue = 0;
    while (!numberString.isEmpty()) {
        QString newSign = numberString.at(0);
        auto itr = signs.find(newSign);
        if (itr == signs.end()) {
            return -1;
        }

        const int newValue = itr->second;
        if (currentSum == 0) {
            currentSum = newValue;
        } else if (newValue > lastSignValue) {
            totalSum += newValue - currentSum; // apply as prefix
            currentSum = 0;
        } else if (newValue == lastSignValue) {
            currentSum += newValue;
        } else {
            totalSum += currentSum;
            currentSum = 0;
        }
        lastSignValue = newValue;
        numberString.remove(0,1);
    }
    return totalSum + currentSum;
}

void Utils::removeLastOccurance(QString& baseStr, const QString matchStr) {
    // qt is weird and gives us reversed positions (last char = 0)
    int turnedPos = baseStr.lastIndexOf(baseStr);
    int normalPos = baseStr.length() - turnedPos - matchStr.length();
    baseStr.remove(normalPos, matchStr.length());
}


QString Utils::createSaveDir(const QString parentDir, const QString dirname) {
    QDir dir = QDir(parentDir);
    dir.mkpath(dirname);
    dir.cd(dirname);
    if (dir.exists()) {
        return dir.absolutePath();
    }
    return QString();
}
