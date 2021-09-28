#ifndef CBPRO_H
#define CBPRO_H

#include <QNetworkAccessManager>

class CBPro : public QObject
{
    Q_OBJECT

public:
    CBPro();
    ~CBPro();

    QNetworkAccessManager* manager;
    //For this dashboard, only the public api is used. So these private API keys are included, and code to sign requests to the private API is included, but not used
    QString key;
    QString b64secret;
    QString passphrase;
    QString apiendpoint;

    QNetworkReply* makeRequest(QString method, QString requestpath, QString querystring="", QString body="");
public slots:
    void replyFinished(QNetworkReply *);
    void setKey(QString k);
    void setB64secret(QString b);
    void setPassphrase(QString p);
signals:
    void replyReceived(QNetworkReply*);
};
#endif // CBPRO_H
