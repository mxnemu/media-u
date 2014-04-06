#include "shortclipcreator.h"

ShortClipCreator::ShortClipCreator(ShortClipCreator::Config* config, QObject* parent) :
    QThread(parent),
    config(config)
{
}

void ShortClipCreator::run() {
    emit done(this->generate());
    deleteLater();
}


std::pair<int, int> ShortClipCreator::Config::adaptRatio(std::pair<int, int> resolution) const {
    return this->adaptRatio(resolution.first, resolution.second);
}

std::pair<int, int> ShortClipCreator::Config::adaptRatio(int originalW, int originalH) const {
    originalW = originalW > 0 ? originalW : 480;
    originalH = originalH > 0 ? originalH : 253;
    int targetW = std::min(480, originalW);
    return std::pair<int,int>(targetW, ((float)originalH / (float)originalW) * (float)targetW);
}

ShortClipCreator::Config::Config() :
    maxSizeMib(3)
{
}

bool ShortClipCreator::Config::isValid() const {
    return true;
}

float ShortClipCreator::Config::timeSpan() const {
    return this->endSec - this->startSec;
}
