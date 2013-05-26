#ifndef MALCLIENT_H
#define MALCLIENT_H

#include <QObject>
#include <qhttpresponse.h>
#include <qhttprequest.h>

class MalClient : public QObject
{
    Q_OBJECT
public:
    explicit MalClient(QObject *parent = 0);

    void setCredentials(QString name, QString password);
    

signals:
    
private slots:
    void setCredentialsAnswer(QHttpRequest *req, QHttpResponse *resp);
};

#endif // MALCLIENT_H
