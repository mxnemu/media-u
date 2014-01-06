#ifndef THUMBNAILCREATOR_H
#define THUMBNAILCREATOR_H

#include <QObject>
#include <qpixmap.h>

class ThumbCreationCallback : public QObject {
    Q_OBJECT
public:
    ThumbCreationCallback(void* data = NULL);
    void* data;

    virtual void start() = 0;
signals:
    void jpegGenerated(const QByteArray img);
};

class ThumbnailCreator
{
public:
    ThumbnailCreator();

    //virtual QPixmap generate(QString file, int second, int width, int height) const;
    virtual ThumbCreationCallback* generateJpeg(QString file, int second, int width, int height, void *callbackData) const = 0;
};

#endif // THUMBNAILCREATOR_H
