#ifndef GIFCREATOR_H
#define GIFCREATOR_H

#include "shortclipcreator.h"

class GifCreator : public ShortClipCreator
{
    Q_OBJECT
public:
    class Config : public ShortClipCreator::Config {
    public:
        Config();
        virtual void describe(nw::Describer &de);

        int resolutionX;
        int resolutionY;
        int framesDropped;
    };

    explicit GifCreator(Config* config, const AvconvConfig& avconvConfig, QObject *parent = 0);

    virtual bool generate();
    bool generate(std::pair<int, int> resolution);
public slots:
};

#endif // GIFCREATOR_H
