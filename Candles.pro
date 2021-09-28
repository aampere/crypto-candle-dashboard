QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cbpro.cpp \
    main.cpp \
    dialog.cpp

HEADERS += \
    cbpro.h \
    dialog.h

FORMS += \
    dialog.ui

INCLUDEPATH += C:\Qt\Tools\OpenSSL\Win_x64\include
LIBS += -LC:\Qt\Tools\OpenSSL\Win_x64\lib -llibcrypto
LIBS += -LC:\Qt\Tools\OpenSSL\Win_x64\lib -llibssl

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
