
exists(.qmake.cache) {
  message("Using .qmake.cache")
}
else {
  message("Doing automated configuration ...")

  unix {
    MY_OS += "unix"
    INCLUDEPATH += /usr/include/qt4/Qt
    exists(/usr/local/include/ta-lib) {
      INCLUDEPATH += /usr/local/include/ta-lib
    }
    exists(/usr/include/ta-lib) {
      INCLUDEPATH += /usr/include/ta-lib
    }
  }

  freebsd-g++ {
    MY_OS += "freebsd"
    INCLUDEPATH += /usr/X11R6/include
  }

  linux-g++ {
    MY_OS += "linux"
  }

  win32 {
    MY_OS += "windows"
  }

  macx {
    MY_OS += "macx"
    # For MacPorts
    exists(/opt/local/include/ta-lib) {
      INCLUDEPATH += /opt/local/include/ta-lib 
    }
    exists(/opt/local/lib) {
      LIBS += -L/opt/local/lib
    }
    # For Fink
    exists(/sw/include/ta-lib) {
      INCLUDEPATH += /sw/include/ta-lib
    }
    exists(/sw/lib) {
      LIBS += -L/sw/lib
    }
  }

  message("Using INCLUDEPATH=$$INCLUDEPATH")
  message("Using LIBS=$$LIBS")

  !exists(qttrader.config) {
	error("Missing configuration file qttrader.config")
  }
	include(qttrader.config)
  
  INCLUDEPATH = $$unique(INCLUDEPATH)
  LIBS = $$unique(LIBS)

  message("Writing .qmake.cache")
  system(echo "INCLUDEPATH=$$INCLUDEPATH" > .qmake.cache)
  system(echo "LIBS=$$LIBS" >> .qmake.cache)
  system(echo "MY_OS=$$MY_OS" >> .qmake.cache)
  system(echo "INSTALL_BIN_DIR=$$INSTALL_BIN_DIR" >> .qmake.cache)
  system(echo "INSTALL_DATA_DIR=$$INSTALL_DATA_DIR" >> .qmake.cache)
  system(echo "INSTALL_DOCS_DIR=$$INSTALL_DOCS_DIR" >> .qmake.cache)
  system(echo "INSTALL_LIB_DIR=$$INSTALL_LIB_DIR" >> .qmake.cache)

  message("Writing lib/qttrader_defines.h")
  win32 {
    system(echo $${LITERAL_HASH}ifndef qttrader_DEFINES_H > lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}define qttrader_DEFINES_H >> lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_BIN_DIR \"$$INSTALL_BIN_DIR\" >> lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DATA_DIR \"$$INSTALL_DATA_DIR\" >> lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DOCS_DIR \"$$INSTALL_DOCS_DIR\" >> lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_LIB_DIR \"$$INSTALL_LIB_DIR\" >> lib/qttrader_defines.h)
    system(echo $${LITERAL_HASH}endif >> lib/qttrader_defines.h)
  } else {
    system(echo \"$${LITERAL_HASH}ifndef qttrader_DEFINES_H\" > lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}define qttrader_DEFINES_H\" >> lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_BIN_DIR \\\"$$INSTALL_BIN_DIR\\\"\" >> lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DATA_DIR \\\"$$INSTALL_DATA_DIR\\\"\" >> lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DOCS_DIR \\\"$$INSTALL_DOCS_DIR\\\"\" >> lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_LIB_DIR \\\"$$INSTALL_LIB_DIR\\\"\" >> lib/qttrader_defines.h)
    system(echo \"$${LITERAL_HASH}endif\" >> lib/qttrader_defines.h)
  }
}
message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")
message("Operating system: $$MY_OS")
!isEmpty(QMAKESPEC) {
  message("QMAKESPEC: $$QMAKESPEC")
}

TEMPLATE = subdirs

# compile qttrader lib
SUBDIRS += lib

# compile app
SUBDIRS += src

# compile plugins

SUBDIRS += plugins/CDL
SUBDIRS += plugins/CSV
SUBDIRS += plugins/CurveHistogram
SUBDIRS += plugins/CurveLine
SUBDIRS += plugins/CurveOHLC
SUBDIRS += plugins/DBStock
SUBDIRS += plugins/DBSymbol
SUBDIRS += plugins/Group
SUBDIRS += plugins/Indicator
SUBDIRS += plugins/MACD
SUBDIRS += plugins/MarkerBuy
SUBDIRS += plugins/MarkerHLine
SUBDIRS += plugins/MarkerRetracement
SUBDIRS += plugins/MarkerSell
SUBDIRS += plugins/MarkerText
SUBDIRS += plugins/MarkerTLine
SUBDIRS += plugins/MarkerVLine
SUBDIRS += plugins/OHLC
SUBDIRS += plugins/STOCHF
SUBDIRS += plugins/STOCHS
SUBDIRS += plugins/Volume
SUBDIRS += plugins/YahooHistory

!win32 {
  # install docs and i18n
 # SUBDIRS += docs
}
