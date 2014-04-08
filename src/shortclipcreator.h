#ifndef SHORTCLIPCREATOR_H
#define SHORTCLIPCREATOR_H

#include <QThread>
#include "nwutils.h"

class ShortClipCreator : public QThread
{
    Q_OBJECT
public:
    class Config {
    public:
        Config();
        virtual ~Config();

        // concrete case data TODO may move to different container
        // but I'm planning to make all other settings changable for 1 case, too
        // so maybe, don't move it
        QString videoPath;
        QString outputPath;
        float startSec;
        float endSec;

        QString dir;
        QString name;
        std::pair<int,int> resolution;
        float maxSizeMib;

        virtual void describe(nw::Describer& de);
        bool isValid() const;
        float timeSpan() const;
        std::pair<int, int> adaptRatio(const std::pair<int, int> resolution) const;
        std::pair<int, int> adaptRatio(int originalW, int originalH) const;

    private:
        void fixInvalidMembers();
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
