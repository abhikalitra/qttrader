TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER
MOC_DIR += build
OBJECTS_DIR += build

HEADERS        += CSV.h
SOURCES        += CSV.cpp
HEADERS        += CSVFormat.h
SOURCES        += CSVFormat.cpp
HEADERS        += CSVFunction.h
SOURCES        += CSVFunction.cpp
HEADERS        += CSVThread.h
SOURCES        += CSVThread.cpp
HEADERS        += Delimiter.h
SOURCES        += Delimiter.cpp
HEADERS        += GUIWidget.h
SOURCES        += GUIWidget.cpp
HEADERS        += Quote.h
SOURCES        += Quote.cpp

target.path = /usr/local/lib/QTTRADER/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network


!exists(./build){
  system(echo mkdir build)
}
