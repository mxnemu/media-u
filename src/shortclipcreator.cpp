#include "shortclipcreator.h"

ShortClipCreator::ShortClipCreator(QObject* parent) :
    QThread(parent)
{
}


std::pair<int, int> ShortClipCreator::Config::adaptRatio(const std::pair<int, int> originalResoution) {
    return originalResoution; // TODO IMPL
}

bool ShortClipCreator::Config::isValid() const {
}
