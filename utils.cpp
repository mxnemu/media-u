#include "utils.h"
#include <QStringList>

Utils::Utils()
{
}

int Utils::querySimiliarity(const QString a, const QString b) {
    QRegExp wordSplitRegex("(\\b)?.+?(\\s|\\b|$)");
    wordSplitRegex.indexIn(a);
    QStringList aWords = wordSplitRegex.capturedTexts();
    wordSplitRegex.indexIn(b);
    QStringList bWords = wordSplitRegex.capturedTexts();

    int matches = 0;
    for (int i=0; i < aWords.length(); ++i) {
        const QString& aWord = aWords.at(i).trimmed();
        for (int j=0; j < bWords.length(); ++j) {
            const QString bWord = bWords.at(i).trimmed();
            if (bWord == aWord) {
                matches += 1;
                break;
            }
        }
    }
    return matches;
}
