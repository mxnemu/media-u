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

        QString videoCodec;
        QString audioCodec;
        QString extension;
        int qualityCrf;

    private:
        QStringList videoCodecArgs() const;
    };

    VideoClipCreator(Config* config, QObject* parent);
    bool generate();
};

#endif // VIDEOCLIPCREATOR_H
