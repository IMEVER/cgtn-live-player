#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T02:13:50
#
#-------------------------------------------------

# libgstreamer-plugins-bad0.10-0

QT       += core gui gui-private dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(lib/singleapplication/singleapplication.pri)

DEFINES += MEDIA_MPV
include(lib/media/media.pri)

DEFINES += QAPPLICATION_CLASS=QApplication

TARGET = cgtn-live-player
TEMPLATE = app

CONFIG += link_pkgconfig

unix{
    PKGCONFIG +=
}

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/playerWidget.cpp \
    src/filterWidget.cpp \
    src/app.cpp \
    src/about.cpp \
    src/player.cpp \
    src/aboutdialog.cpp \
    src/channelconf.cpp \
    src/conf.cpp \
    src/listTv.cpp \
    src/listGroup.cpp \
    src/logger.cpp \
    src/mpris.cpp

HEADERS  += src/mainwindow.h \
    src/playerWidget.h \
    src/filterWidget.h \
    src/app.h \
    src/about.h \
    src/player.h \
    src/aboutdialog.h \
    src/channelconf.h \
    src/conf.h \
    src/listTv.h \
    src/listGroup.h \
    src/logger.h \
    src/mpris.h

FORMS +=

RESOURCES += \
    resource.qrc
