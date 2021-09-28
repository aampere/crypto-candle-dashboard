#include "dialog.h"
#include "ui_dialog.h"
#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QPainter>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>


Dialog::Dialog(QWidget *parent)
    : QDialog(parent),  cbpro(new CBPro()),
      pairs(QList<QString>()),
      granularity(60),
      refreshindex(0),
      refreshinterval(2000),
      refreshtimer(this),
      ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(cbpro, &CBPro::replyReceived, this, &Dialog::replyFinished);

    pairs << "BTC-USD"<<"ETH-USD"<<"LTC-USD"<<"BCH-USD"<<"EOS-USD"<<"OXT-USD"<<"MKR-USD"<<"XLM-USD"<<"ATOM-USD"<<"XTZ-USD"<<"ETC-USD"<<"OMG-USD"<<"ZEC-USD"<<"LINK-USD"<<"ZRX-USD"<<"ALGO-USD"<<"DAI-USD"<<"KNC-USD"<<"COMP-USD"<<"BAND-USD"<<"NMR-USD"<<"CGLD-USD"<<"UMA-USD"<<"LRC-USD"<<"YFI-USD"<<"UNI-USD"<<"REN-USD"<<"BAL-USD"<<"NU-USD"<<"FIL-USD"<<"AAVE-USD"<<"GRT-USD"<<"BNT-USD"<<"SNX-USD";
    ui->dashlist->setStyleSheet("QListWidget{background: black;}");
    for(auto p=pairs.begin(); p!=pairs.end(); ++p)
    {
        QListWidgetItem* li = new QListWidgetItem();
        dashitems[*p] = li;
        allcandles[*p] = QList<Candle>();
        ui->dashlist->addItem(li);
    }
    ui->dashlist->setIconSize(QSize(600,200));

    //Private signed API requests are not used in this dashboard, but here is a basic dialog for entering them if desired.
    /*
    QDialog* apikeydialog = new QDialog(this);
    apikeydialog->setWindowTitle("Enter Coinbase Pro API keys");
    apikeydialog->setMinimumWidth(400);
    QVBoxLayout* layout = new QVBoxLayout();
    QLineEdit* keyfield = new QLineEdit("key");
    layout->addWidget(keyfield);
    QLineEdit* b64secretfield = new QLineEdit("b64secret");
    layout->addWidget(b64secretfield);
    QLineEdit* passphrasefield = new QLineEdit("passphrase");
    layout->addWidget(passphrasefield);
    QPushButton* okbutton = new QPushButton("OK");
    layout->addWidget(okbutton);
    apikeydialog->setLayout(layout);
    connect(okbutton, &QPushButton::clicked, apikeydialog, &QDialog::accept);
    if(apikeydialog->exec()) {
        cbpro->setKey(keyfield->text());
        cbpro->setB64secret(b64secretfield->text());
        cbpro->setPassphrase(passphrasefield->text());
        refreshtimer.setInterval(refreshinterval);
        connect(&refreshtimer, &QTimer::timeout, this, &Dialog::refreshNextChart);
        refreshtimer.start();
    }
    */

    refreshtimer.setInterval(refreshinterval);
    connect(&refreshtimer, &QTimer::timeout, this, &Dialog::refreshNextChart);
    refreshtimer.start();
}
Dialog::~Dialog()
{
    delete ui;
}

void Dialog::replyFinished(QNetworkReply * reply)
{
    QByteArray replycontents = reply->readAll();
    QString path = reply->request().url().path();
    if(path.endsWith("candles"))
    {
        //eg url=   "/products/BTC-USD/candles"
        QStringList parts = path.split("/");
        if(parts.size()!=4) {
            qDebug() << "Received candles reply, but could not parse";
            return;
        }
        QJsonParseError jsonerror;
        QString pair = parts[2]; //eg "BTC-USD"
        qDebug() << QString("refreshing %0").arg(pair);
        QList<Candle> replycandles;
        QJsonDocument dummydoc = QJsonDocument::fromJson(replycontents,&jsonerror);
        if(dummydoc.isNull()) {
            qDebug() << "Candle reply parsing failed:";
            qDebug() << jsonerror.errorString();
            return;
        }
        if(!dummydoc.isArray()) {
            qDebug() << "Candle reply was not an array";
            return;
        }
        QJsonArray jc = dummydoc.array();
        if(jc.isEmpty()) {
            qDebug() << "Candle array was empty";
            return;
        }
        for(auto c=jc.begin(); c!=jc.end(); ++c) {
            if(!c->isArray()) {
                qDebug() << "An item in the candle reply was not an array";
                continue;
            }
            QJsonArray candlearr = c->toArray();
            if(candlearr.size() != 6) {
                qDebug() << "An item in the candle reply was not size 6";
            }
            Candle candle;
            candle.time = candlearr.at(0).toInt();
            candle.low = candlearr.at(1).toDouble();
            candle.high = candlearr.at(2).toDouble();
            candle.open = candlearr.at(3).toDouble();
            candle.close = candlearr.at(4).toDouble();
            candle.volume = candlearr.at(5).toDouble();
            if(c!=jc.begin()) {
                while(candle.time != replycandles[replycandles.size()-1].time-granularity) {
                    Candle emptycandle;
                    emptycandle.time = replycandles[replycandles.size()-1].time-granularity;
                    emptycandle.low = candle.open;
                    emptycandle.high = candle.open;
                    emptycandle.open = candle.open;
                    emptycandle.close = candle.open;
                    emptycandle.volume = 0.0;
                    replycandles.append(emptycandle);
                }
            }
            replycandles.append(candle);
        }
        allcandles[pair] = replycandles;
        updateItemChart(pair);
    }
}
void Dialog::refreshNextChart()
{
    QString pair = pairs[refreshindex];
    cbpro->makeRequest("GET", QString("/products/%0/candles").arg(pair),QString("start=%0&end=%1&granularity=%2").arg(QDateTime::currentDateTimeUtc().addSecs(-9000).toString(Qt::ISODate)).arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate)).arg(granularity));
    refreshindex = (refreshindex+1)%pairs.size();
}

void Dialog::updateItemChart(QString pair)
{
    if(!allcandles.contains(pair))
    {
        qDebug()<< "updateItemChart() bad pair";
        return;
    }
    QList<Candle>& candles(allcandles[pair]);
    //set new Icon
    dashitems[pair]->setIcon(getIconFromCandles(pair, candles));
}
QIcon Dialog::getIconFromCandles(QString pair, QList<Candle>& candles)
{
    int w = 600;
    int h = 120;
    int candlewidth = 3;
    int candlespacing = 1; //gap between candles
    int vertmargin = 18;
    int horomargin = 58;
    int tickw = 3; //tickwidth for yaxis ticks
    int tickh = 2;
    int timetickinterval = 20; //every nth candle draw a time tick
    double volmag = 0.25; //volume bars will occupy this fraction of the height of the chart
    QPointF topright(w-horomargin,vertmargin);
    QPointF bottomright(w-horomargin,h-vertmargin);
    QPointF topleft(0,vertmargin);
    QPointF bottomleft(0,h-vertmargin);
    QColor bgcol(0,0,0);
    QColor axiscol(200,200,200);
    QColor green(0,255,0);
    QColor red(255,0,0);
    QColor volcol(180,180,180,130); //note alpha
    QPixmap pixmap(w, h);
    QPainter p(&pixmap);
    QFont font = p.font();
    QFontMetrics fm(font);
    int fonth = fm.height();

    //analyze candles
    double maxhigh = 0.0;
    double minlow = std::numeric_limits<double>::max();
    double maxvol = 0.0;
    for(auto candle=candles.begin(); candle!=candles.end(); candle++) {
        maxhigh = std::max(maxhigh,candle->high);
        minlow = std::min(minlow,candle->low);
        maxvol = std::max(maxvol,candle->volume);
    }
    double centerprice = (maxhigh+minlow)*0.5;
    double onepercent = 0.01*centerprice;

    //fill bg
    p.setBrush(QBrush(bgcol));
    p.fillRect(0,0,w,h,QBrush(bgcol));
    //draw axes
    QPen axispen(axiscol);
    p.setPen(axispen);
    p.drawLine(topleft,topright);
    p.drawLine(topright,bottomright);
    p.drawLine(bottomright,bottomleft);
    //draw title
    p.drawText(w-horomargin+tickw+2,fm.height()+0,pair);
    //draw price ticks
    double pricetick = centerprice;
    p.drawLine(w-horomargin, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin), w-horomargin+tickw, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin));
    p.drawText(w-horomargin+tickw+2, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin)+fonth*0.3, QString("%0").arg(centerprice));
    int i=1;
    while(pricetick+onepercent<maxhigh) {
        pricetick += onepercent;
        p.drawLine(w-horomargin, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin), w-horomargin+tickw, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin));
        p.drawText(w-horomargin+tickw+2, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin)+fonth*0.3, QString("%0%").arg(i));
        ++i;
    }
    pricetick = centerprice;
    i=1;
    while(pricetick-onepercent>minlow) {
        pricetick -= onepercent;
        p.drawLine(w-horomargin, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin), w-horomargin+tickw, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin));
        p.drawText(w-horomargin+tickw+2, h - vertmargin - (pricetick-minlow)/(maxhigh-minlow)*(h-2*vertmargin)+fonth*0.3, QString("%0%").arg(i));
        ++i;
    }

    //draw candles
    int candleright = w-horomargin-candlespacing; //right edge of the candle
    int candleleft = candleright-candlewidth; //left edge of the candle
    int candlecenter = candleleft+candlewidth/2; //For the wick of the candle
    i=0;
    for(auto candle=candles.begin(); candle!=candles.end(); candle++) {
        //draw candle
        double top = std::max(candle->open, candle->close); //in price units
        double bottom = std::min(candle->open, candle->close); //in price units
        int candletop = h - vertmargin - (top-minlow)/(maxhigh-minlow)*(h-2*vertmargin);
        int candlebottom = h - vertmargin - (bottom-minlow)/(maxhigh-minlow)*(h-2*vertmargin);
        int wicktop = h - vertmargin - (candle->high-minlow)/(maxhigh-minlow)*(h-2*vertmargin);
        int wickbottom = h - vertmargin - (candle->low-minlow)/(maxhigh-minlow)*(h-2*vertmargin);
        QBrush candlebrush(candle->close > candle->open ? green : red);
        QPen candlepen(candle->close > candle->open ? green : red);
        if(candle->volume!=0.0){
            p.setPen(candlepen);
            p.fillRect(candleleft,candletop,candlewidth,std::max(candlebottom-candletop,1),QBrush(candlebrush));
            p.drawLine(candlecenter,wicktop,candlecenter,wickbottom);
        }
        //draw time tick
        p.setPen(axispen);
        if(i%timetickinterval == 0) {
            p.drawLine(candlecenter,h-vertmargin,candlecenter,h-vertmargin+tickh);
            QString timestring = QDateTime::fromSecsSinceEpoch(candle->time).time().toString("HH:mm");
            int labelw = fm.horizontalAdvance(timestring);
            int labelh = fm.height();
            p.drawText(candlecenter-labelw*0.5,h-vertmargin+tickh+labelh,timestring);
        }
        //draw volume
        QBrush volbrush(volcol);
        p.fillRect(candleleft,h-vertmargin-(candle->volume/maxvol)*(h-2*vertmargin)*volmag,candlewidth,(candle->volume/maxvol)*(h-2*vertmargin)*volmag,QBrush(volbrush));

        //move candle left for next iteration
        candleright = candleright - candlewidth - candlespacing;
        candleleft = candleright-candlewidth;
        candlecenter = candleleft+candlewidth/2;
        ++i;
    }
    return QIcon(pixmap);
}

