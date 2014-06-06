#-------------------------------------------------
#
# Project created by QtCreator 2013-11-15T17:17:06
#
#-------------------------------------------------

QT       += core widgets network sql

TARGET = Serizer
VERSION = 0.01
TEMPLATE = app

ICON = resources/logo.png

macx {
QT += macextras
#QMAKE_LFLAGS += -F$$OUT_PWD/../Third-part/qupdater
LIBS += -lBetaSeriesApi
LIBS += -lsqlite3 -framework Cocoa -framework Foundation # -framework qUpdater
}

#QMAKE_INFO_PLIST = file.plist

SOURCES += \
    main.cpp\
    ui/mainwindow.cpp \
    ui/homewidget.cpp \
    ui/serieswidget.cpp \
    ui/resizefilter.cpp \
    ui/preferencesdialog.cpp \
    ui/memberconnectdialog.cpp \
    ui/unseenepisodewidget.cpp \
    databasemanager.cpp \
    trayicon.cpp \
    trayiconwidget.cpp

HEADERS  += \
    ui/mainwindow.h \
    ui/homewidget.h \
    ui/serieswidget.h \
    ui/resizefilter.h \
    ui/memberconnectdialog.h \
    ui/preferencesdialog.h \
    ui/unseenepisodewidget.h \
    databasemanager.h \
    trayicon.h \
    trayiconwidget.h

FORMS    += \
    ui/mainwindow.ui \
    ui/memberconnectdialog.ui \
    ui/homewidget.ui \
    ui/serieswidget.ui \
    ui/preferencesdialog.ui \
    trayiconwidget.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    languages/Serizer_fr.ts

INSTALL += target

################################################################################
# INCLUDE openssl
################################################################################
win32:LIBS += -L$$IN_PWD/../Third-part/OpenSSL-Win32/ -leay32

################################################################################
# INCLUDE qupdater
################################################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Third-part/qupdater/src/release/ -lqUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Third-part/qupdater/src/debug/ -lqUpdater
else:unix:LIBS += -L$$OUT_PWD/../Third-part/qupdater/src -lqUpdater

macx {
APP_DESTDIR = $${TARGET}.app/Contents/MacOS
APP_LIBDIR = $${TARGET}.app/Contents/Frameworks

QMAKE_POST_LINK += mkdir -p $${APP_LIBDIR};

QMAKE_POST_LINK += cp -r $$OUT_PWD/../Third-part/qupdater/src/libqUpdater.dylib $${APP_LIBDIR} ;
QMAKE_POST_LINK += install_name_tool -id @executable_path/../Frameworks/libqUpdater.dylib $${APP_LIBDIR}/libqUpdater.dylib ;
QMAKE_POST_LINK += install_name_tool -change libqUpdater.1.dylib @executable_path/../Frameworks/libqUpdater.dylib $${APP_DESTDIR}/$${TARGET};
}

INCLUDEPATH += $$PWD/../Third-part/qupdater/src
DEPENDPATH += $$PWD/../Third-part/qupdater/src

PRE_TARGETDEPS += $$OUT_PWD/../Third-part/qupdater/src

################################################################################
# INCLUDE BetaSeriesApi
################################################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Third-part/BetaSeriesApi/src/release/ -lBetaSeriesApi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Third-part/BetaSeriesApi/src/debug/ -lBetaSeriesApi
else:unix: LIBS += -L$$OUT_PWD/../Third-part/BetaSeriesApi/src -lBetaSeriesApi

macx {
QMAKE_POST_LINK += cp -r $$OUT_PWD/../Third-part/BetaSeriesApi/src/libBetaSeriesApi.dylib $${APP_LIBDIR} ;
QMAKE_POST_LINK += install_name_tool -id @executable_path/../Frameworks/libBetaSeriesApi.dylib $${APP_LIBDIR}/libBetaSeriesApi.dylib ;
QMAKE_POST_LINK += install_name_tool -change libBetaSeriesApi.1.dylib @executable_path/../Frameworks/libBetaSeriesApi.dylib $${APP_DESTDIR}/$${TARGET};
}

INCLUDEPATH += $$PWD/../Third-part/BetaSeriesApi/src
DEPENDPATH += $$PWD/../Third-part/BetaSeriesApi/src

PRE_TARGETDEPS += $$OUT_PWD/../Third-part/BetaSeriesApi/src
