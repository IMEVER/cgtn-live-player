#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T02:13:50
#
#-------------------------------------------------

# libgstreamer-plugins-bad0.10-0

QT       += core gui multimediawidgets multimedia gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cgtn-live-player
TEMPLATE = app

CONFIG += link_pkgconfig

unix{
    PKGCONFIG +=
}

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/app.cpp \
    src/about.cpp \
    src/player.cpp \
    src/playbutton.cpp \
    src/volumelabel.cpp \
    src/aboutdialog.cpp

HEADERS  += src/mainwindow.h \
    src/app.h \
    src/about.h \
    src/player.h \
    src/playbutton.h \
    src/volumelabel.h \
    src/aboutdialog.h

DISTFILES +=
FORMS +=

RESOURCES += \
    resource.qrc
