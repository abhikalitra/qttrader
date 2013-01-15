
#DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lQTTRADER -lta_common -lta_func -lta_abstract
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += ControlWidget.h
SOURCES += ControlWidget.cpp
HEADERS += Indicator.h
SOURCES += Indicator.cpp
HEADERS += IndicatorDialog.h
SOURCES += IndicatorDialog.cpp
HEADERS += LengthButton.h
SOURCES += LengthButton.cpp
HEADERS += PlotWidget.h
SOURCES += PlotWidget.cpp
HEADERS +=
SOURCES +=

target.path = "$${INSTALL_PLUGIN_DIR}"
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network

OTHER_FILES += \
    gui.qml

!exists(./build){
  system(echo mkdir build)
}
