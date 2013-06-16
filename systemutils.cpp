#include "systemutils.h"

#ifdef __unix__
#include <sys/resource.h>
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

int SystemUtils::setProcessPriority(QProcess &process, int nice) {
#ifdef __unix__
    return setpriority(PRIO_PROCESS, process.pid(), nice);
#else
    return 0;
#endif
}
