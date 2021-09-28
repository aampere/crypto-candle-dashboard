# crypto-candle-dashboard
Display and compare many candle charts for different cryptocurrencies. Drag and drop to rearrange charts of interest to compare easily in a vertical stack. Charts update every minute.  
A candle chart is a type of chart used in stock and currency trading that graphically shows the opening, closing, high, and low prices of the stock/currency over some number of intervals. The motivation behind this dashboard was to have an easy way to monitor many cryptocurrency markets at once, and to label the y-axis with a percent-change scale rather than an absolute scale. This allows the user to easily see which of many currencies are particularly volatile currently, and if any any have become "unpinned" from the other currencies. 

## Screenshot
![Screenshot](Candle%20Dashboard%20screenshot.JPG)

## Code notes
Built with Qt/C++. Requests are made to the public Coinbase Pro API and do not require API keys, but this code also incudes the ability to send signed private API requests if API keys are provided.

## Build for Windows x64
Download the zip and open executable to run.