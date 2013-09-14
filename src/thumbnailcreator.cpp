#include "thumbnailcreator.h"

ThumbCreationCallback::ThumbCreationCallback(void* data) : QObject(),
    data(data)
{

}

ThumbnailCreator::ThumbnailCreator()
{
}

/*
QPixmap ThumbnailCreator::generate(QString file, int second, int width, int height) const {
    QPixmap pixmap;
    pixmap.loadFromData(generateJpeg(file, second, width, height), "JPEG");
    return pixmap;
}
*/
