#include "shortclipcreator.h"
#include <QStandardPaths>
#include "utils.h"

ShortClipCreator::ShortClipCreator(ShortClipCreator::Config* config, const AvconvConfig &avconvConfig, QObject* parent) :
    QThread(parent),
    config(config),
    avconvConfig(avconvConfig)
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

void ShortClipCreator::Config::fixInvalidMembers() {
    if (this->name.isEmpty()) {
        this->name = "$(tvShow)/$(file) - $(startM)m$(startS)s:$(endM)m$(endS)s - $(nowDate).$(ext)";
    }
    if (this->dir.isEmpty()) {
        QStringList imageDirs = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

        if (!imageDirs.isEmpty()) {
            QString imageDir = imageDirs.first();
            this->dir = Utils::createSaveDir(imageDir, "snapshots");
        }
    }
}

ShortClipCreator::Config::Config() :
    resolution(std::pair<int,int>(0,0)),
    maxSizeMib(3)
{
}

ShortClipCreator::Config::~Config()
{
}

void ShortClipCreator::Config::describe(nw::Describer& de) {
    de.describe("resolutionX", resolution.first);
    de.describe("resolutionY", resolution.second);
    de.describe("maxSizeMib", maxSizeMib);

    this->fixInvalidMembers();
}

bool ShortClipCreator::Config::isValid() const {
    return true;
}

float ShortClipCreator::Config::timeSpan() const {
    return this->endSec - this->startSec;
}
