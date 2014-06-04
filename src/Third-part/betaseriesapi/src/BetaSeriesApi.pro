#-------------------------------------------------
#
# Project created by QtCreator 2014-02-26T22:12:02
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = BetaSeriesApi
TEMPLATE = lib

DEFINES += BETASERIESAPI_LIBRARY

SOURCES += \
    betaseriesapi.cpp

HEADERS +=\
    betaseriesapi.h \
    betaseriesapi_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
