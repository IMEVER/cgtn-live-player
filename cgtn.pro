#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T02:13:50
#
#-------------------------------------------------

QT       += core gui multimediawidgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cgtn
TEMPLATE = app

CONFIG += link_pkgconfig

unix{
    PKGCONFIG += dtkbase dtkwidget dtkutil
}

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/app.cpp \
    src/about.cpp \
    src/player.cpp \
    src/playbutton.cpp \
    src/volumelabel.cpp

HEADERS  += src/mainwindow.h \
    src/app.h \
    src/about.h \
    src/player.h \
    src/playbutton.h \
    src/volumelabel.h

DISTFILES +=
FORMS +=

RESOURCES += \
    resource.qrc
