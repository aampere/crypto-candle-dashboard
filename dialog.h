#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QListWidgetItem>
#include <QTimer>
#include "cbpro.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

struct Candle
{
    int time, granularity; //start time in UTC timestamp,  slice of time (eg, 60=1min candles)
    double low, high, open, close, volume;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    CBPro* cbpro;

    QList<QString> pairs; //eg "BTC-USD"
    QMap<QString, QListWidgetItem*> dashitems; //pair : list item
    QMap<QString, QList<Candle> > allcandles;
    int granularity; //candle granularity. 60.

    int refreshindex;
    int refreshinterval; //msec
    QTimer refreshtimer;

    QList<Candle> dummycandles;
public slots:
    void replyFinished(QNetworkReply *);
    void refreshNextChart();

    void updateItemChart(QString pair);
    QIcon getIconFromCandles(QString pair, QList<Candle>& candles);
private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
