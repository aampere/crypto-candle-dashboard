#include "cbpro.h"

#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QNetworkReply>
#include <QUrlQuery>

CBPro::CBPro()
    : manager(new QNetworkAccessManager()),
      key(""),
      b64secret(""),
      passphrase(""),
      apiendpoint("api.pro.coinbase.com")
{
    connect(manager, &QNetworkAccessManager::finished, this, &CBPro::replyFinished);
}
CBPro::~CBPro()
{

}

QNetworkReply* CBPro::makeRequest(QString method, QString requestpath, QString querystring, QString body)
{
    QNetworkRequest request;
    QUrl url;
    url.setScheme("https");
    url.setHost(apiendpoint);
    url.setPath(requestpath);
    if(querystring!="")
        url.setQuery(querystring);
    request.setUrl(url);
    QByteArray access_key(key.toUtf8());
    QByteArray access_timestamp(QString("%1").arg(QDateTime::currentSecsSinceEpoch()).toUtf8());
    QByteArray access_passphrase(passphrase.toUtf8());
    QByteArray access_sign_prehash("");
    access_sign_prehash += access_timestamp;
    access_sign_prehash += method.toUtf8();
    access_sign_prehash += requestpath.toUtf8();
    QByteArray access_sign = QMessageAuthenticationCode::hash(access_sign_prehash, QByteArray::fromBase64(b64secret.toUtf8()), QCryptographicHash::Sha256).toBase64();
    access_sign_prehash += body.toUtf8(); //Body. No body for GET
    request.setRawHeader(QByteArray("CB-ACCESS-KEY"), access_key);
    request.setRawHeader(QByteArray("CB-ACCESS-SIGN"), access_sign);
    request.setRawHeader(QByteArray("CB-ACCESS-TIMESTAMP"), access_timestamp);
    request.setRawHeader(QByteArray("CB-ACCESS-PASSPHRASE"), access_passphrase);
    QNetworkReply *reply = manager->get(request);
    return reply;
}
void CBPro::replyFinished(QNetworkReply * reply)
{
    emit replyReceived(reply);
}
void CBPro::setKey(QString k) {
    key = k;
}
void CBPro::setB64secret(QString b) {
    b64secret = b;
}
void CBPro::setPassphrase(QString p) {
    passphrase = p;
}
