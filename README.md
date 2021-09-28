# Crypto Candle Dashboard
Display and compare many candle charts for different cryptocurrencies. Drag and drop to rearrange charts of interest to compare easily in a vertical stack. Charts update every minute.  
A candle chart is a type of chart used in stock and currency trading that graphically shows the opening, closing, high, and low prices of the stock/currency over some number of intervals. The motivation behind this dashboard was to have an easy way to monitor many cryptocurrency markets at once, and to label the y-axis with a percent-change scale rather than an absolute scale. This allows the user to easily see which of many currencies are particularly volatile currently, and if any any have become "unpinned" from the other currencies. 

## Screenshot
![Candle Dashboard screenshot](https://user-images.githubusercontent.com/66134580/135158262-64b33489-4650-4d7e-b85a-48912bbe0506.JPG)

## Code notes
Built with Qt/C++. Requests are made to the public Coinbase Pro API and do not require API keys, but this code also incudes the ability to send signed private API requests if API keys are provided.

## Setup
The dashboard requires Qt to build. Qt is a cross-platform framework built on C++ that provides a great UI library. 
- Create a Qt account. https://login.qt.io/register You may need to describe what you're using Qt for. It is possible to obtain a non-commercial Qt license for free in this process, which is what I use.
- Install Qt: https://doc.qt.io/qt-5/gettingstarted.html.
  - I recommend installing Qt Creator, Qt's dedicated IDE, during this process, although other IDEs and workflows can work with configuration.
  - Choose to include the optional OpenSSL Toolkit. Note that Qt says it will not create the OpenSSL environment, the user must do it. This is due to liability, or something, I believe. Because Qt has not added the relevant DLLs to path, you must either add them to your path or copy the DLLs to the debug/release directory. See https://stackoverflow.com/questions/53805704/tls-initialization-failed-on-get-request/53809217.
- Open `Candles.pro' in Qt Creator.
- Run.

## Build for Windows x64
You can download a build at http://fastgrav.com/misc/Crypto%20Candles%20Dashboard_windows64.zip.
(This website is run by me, for my other project FastGrav. Sorry, I don't have and SSL certificate.)
To run, unzip the archive and open `Candles.exe'.
