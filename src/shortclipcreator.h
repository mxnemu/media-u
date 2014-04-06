#ifndef SHORTCLIPCREATOR_H
#define SHORTCLIPCREATOR_H

#include <QThread>

class ShortClipCreator : public QThread
{
    Q_OBJECT
public:
    ShortClipCreator(QObject* parent = NULL);

    class Config {
    public:
        QString videoPath;
        QString outputPath;
        float startSec;
        float endSec;
        std::pair<int,int> resolution;
        float maxSizeMib;

        std::pair<int,int> adaptRatio(const std::pair<int,int> originalResoution);
        bool isValid() const;
    };
};

#endif // SHORTCLIPCREATOR_H
