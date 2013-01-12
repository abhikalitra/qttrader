TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER -lta_common -lta_func -lta_abstract
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += OHLC.h
SOURCES += OHLC.cpp
HEADERS += OHLCDialog.h
SOURCES += OHLCDialog.cpp

target.path = /usr/local/lib/QTTRADER/plugins
INSTALLS += target

QT += core
QT += gui

!exists(./build){
  system(echo mkdir build)
}
