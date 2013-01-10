TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER -lta_common -lta_func -lta_abstract
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += MarkerText.h
SOURCES += MarkerText.cpp
HEADERS += MarkerTextDialog.h
SOURCES += MarkerTextDialog.cpp

target.path = /usr/local/lib/QTTRADER/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network

!exists(./build){
  system(echo mkdir build)
}
