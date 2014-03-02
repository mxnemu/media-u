#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <qhttprequest.h>
#include <qhttpserver.h>

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent = 0);

    virtual bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp) = 0;
    virtual void initFromQuery(const QString& initString);
    virtual bool conformsTo(QString) const { return false; }
    
signals:
    
public slots:
    
};

#endif // PAGE_H
