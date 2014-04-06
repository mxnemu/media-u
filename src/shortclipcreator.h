#ifndef SHORTCLIPCREATOR_H
#define SHORTCLIPCREATOR_H

#include <QThread>

class ShortClipCreator : public QThread
{
    Q_OBJECT
public:
    class Config {
    public:
        Config();

        QString videoPath;
        QString outputPath;
        float startSec;
        float endSec;
        std::pair<int,int> resolution;
        float maxSizeMib;

        bool isValid() const;
        float timeSpan() const;
        std::pair<int, int> adaptRatio(const std::pair<int, int> resolution) const;
        std::pair<int, int> adaptRatio(int originalW, int originalH) const;
    };

    ShortClipCreator(ShortClipCreator::Config* config, QObject* parent = NULL);

    virtual bool generate() = 0;
    void run();

signals:
    void done(bool);
protected:
    const Config* config;
};

#endif // SHORTCLIPCREATOR_H
