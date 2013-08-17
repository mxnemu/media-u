#include "mainbackgroundwidget.h"
#include <QPainter>

MainBackgroundWidget::MainBackgroundWidget(QWidget *parent) :
    QWidget(parent)
{
}

void MainBackgroundWidget::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    if (!backgroundImage.isNull()) {
        p.drawPixmap(0,0, width(), height(), backgroundImage);
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
    QPainter tinter(&this->backgroundImage);
    QColor tintColor = QColor(255, 255, 255, 230);
    //tinter.setCompositionMode(QPainter::CompositionMode_Overlay);
    tinter.fillRect(this->backgroundImage.rect(), tintColor);
    tinter.end();
}
