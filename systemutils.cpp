#include "systemutils.h"

#ifdef __unix__
#elif defined(windows)
#endif

#include <stdlib.h>
#include <stdio.h>


SystemUtils::SystemUtils()
{
}

QString SystemUtils::fileMime(const QString filepath) {
#ifdef __linux__
    char mimeBuffer[128];
    FILE* fp = popen(QString("xdg-mime query filetype %1").arg(filepath).toStdString().data(), "r");
    if (fp == NULL) {
        return QString();
    }
    while (fgets(mimeBuffer, sizeof(mimeBuffer)-1, fp) != NULL) {
        //printf("mime: %s\n", mimeBuffer);
        return mimeBuffer;
    }
    return mimeBuffer;
#endif
    return QString();
}
