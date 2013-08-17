#ifndef MAINBACKGROUNDWIDGET_H
#define MAINBACKGROUNDWIDGET_H

#include <QWidget>

class MainBackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainBackgroundWidget(QWidget *parent = 0);
    
    void paintEvent(QPaintEvent *e);
    void setBackground(QString backgroundUrl);
signals:
    
public slots:
    
private:
    QPixmap backgroundImage;
    QString backgroundUrl;
};

#endif // MAINBACKGROUNDWIDGET_H
