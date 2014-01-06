#ifndef THUMBNAILCREATORAVCONV_H
#define THUMBNAILCREATORAVCONV_H

#include "thumbnailcreator.h"
#include <QProcess>

class ThumbCreationCallbackAvconv : public ThumbCreationCallback {
    Q_OBJECT
public:
    ThumbCreationCallbackAvconv(void* data);

    void start();

    QProcess process;
    QStringList args;

public slots:
    void processFinished(int);
};

class ThumbnailCreatorAvconv : public ThumbnailCreator {
public:
    ThumbnailCreatorAvconv();

    ThumbCreationCallback* generateJpeg(QString file, int second, int width, int height, void *callbackData) const;
};

#endif // THUMBNAILCREATORAVCONV_H
