#ifndef VIDEOCLIPCREATOR_H
#define VIDEOCLIPCREATOR_H

#include "shortclipcreator.h"

class VideoClipCreator : public ShortClipCreator
{
public:
    class Config : public ShortClipCreator::Config {
        friend VideoClipCreator;
    public:
        Config();
        virtual void describe(nw::Describer &de);
        QString getExtension();

    private:
        QString videoCodec;
        QString audioCodec;
        QString extension;
        int qualityCrf;

        QStringList videoCodecArgs() const;
    };

    VideoClipCreator(Config* config, QObject* parent = NULL);
    bool generate();
};

#endif // VIDEOCLIPCREATOR_H
