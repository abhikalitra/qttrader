TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER -lta_common -lta_func -lta_abstract
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += CurveLine.h
SOURCES += CurveLine.cpp
HEADERS += CLBar.h
SOURCES += CLBar.cpp

target.path = "$${INSTALL_PLUGIN_DIR}"
INSTALLS += target

QT += core
QT += gui

!exists(./build){
  system(echo mkdir build)
}
