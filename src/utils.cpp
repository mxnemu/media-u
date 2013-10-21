#include "utils.h"
#include <QStringList>

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

    std::pair<QString, int> result = std::pair<QString, int>(QString(), -1);
    QStringList results;
    int index;
    do {
        //index = result.second + result.first.length(); // the fast & sloppy way
        index = result.second + 1;
        result = commonSliceAtStart(a,b, index);
        if (result.first.length() > 1) {
            results << result.first;
        }
    } while (index < a.length());
    return results;
}

std::pair<QString, int> Utils::commonSliceAtStart(const QString& a, const QString& b, int startIndex) {
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
    return std::pair<QString, int>(lastCommon.trimmed(), startIndex);
}

QString Utils::commonSlicesInStrings(const QStringList &strings) {
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

    const QString* bestResult = commonResults.empty() ? NULL : &commonResults.front();
    foreach (const QString r, commonResults) {
        if (r.length() > bestResult->length()) {
            bestResult = &r;
        }
    }

    if (bestResult) {
        return QString(*bestResult);
    }
    return QString();
}
