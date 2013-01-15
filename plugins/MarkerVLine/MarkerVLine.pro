TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER -lta_common -lta_func -lta_abstract
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += MarkerVLine.h
SOURCES += MarkerVLine.cpp
HEADERS += MarkerVLineDialog.h
SOURCES += MarkerVLineDialog.cpp

target.path = "$${INSTALL_PLUGIN_DIR}"
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network

!exists(./build){
  system(echo mkdir build)
}
