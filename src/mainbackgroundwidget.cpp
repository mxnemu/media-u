#include "mainbackgroundwidget.h"
#include <QPainter>
#include <QPixmap>

MainBackgroundWidget::MainBackgroundWidget(QWidget *parent) :
    QWidget(parent)
{
}

void MainBackgroundWidget::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    if (!backgroundImage.isNull()) {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::HighQualityAntialiasing, true);

        int w = backgroundImage.width();
        int h = height();
        float scale = ((float)h) / (float)backgroundImage.height();
        w = ((float)w)*scale;
        p.drawPixmap(0,0, w, h, backgroundImage);
        if (w < width()) {
            QBrush brush(QColor(Qt::darkRed));

            p.scale(-1, 1);

            p.drawPixmap(QRect(QPoint(-w*2,0), QPoint(-w, height())), backgroundImage);
            p.scale(-1,1);

            qreal gradientEnd = ((float)(width()-w)/(float)w);
            gradientEnd = std::min((qreal)1.f, gradientEnd);
            gradientEnd = std::max((qreal)0.f, gradientEnd);

            QRect grect(QPoint(w,0), QPoint(width(), h));
            QLinearGradient gradient(QPoint(grect.left(), 0), QPoint(grect.left() + w, 0));
            gradient.setColorAt(0, QColor(255,255,255,0));
            gradient.setColorAt(gradientEnd, QColor(Qt::white));
            p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.fillRect(grect, gradient);
        }
    }
    QWidget::paintEvent(e);
}

void MainBackgroundWidget::setBackground(QString backgroundUrl) {
    if (backgroundUrl.isNull()) {
        backgroundImage = QPixmap();
        return;
    }
    this->backgroundUrl = backgroundUrl;
    this->backgroundImage.load(backgroundUrl);
    //QPainter tinter(&this->backgroundImage);
    //QColor tintColor = QColor(255, 255, 255, 230);
    ////tinter.setCompositionMode(QPainter::CompositionMode_Overlay);
    //tinter.fillRect(this->backgroundImage.rect(), tintColor);
    //tinter.end();
}
