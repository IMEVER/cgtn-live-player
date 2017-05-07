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

SOURCES += main.cpp\
        mainwindow.cpp \
    app.cpp \
    about.cpp \
    player.cpp \
    playbutton.cpp \
    volumelabel.cpp

HEADERS  += mainwindow.h \
    app.h \
    about.h \
    player.h \
    playbutton.h \
    volumelabel.h

DISTFILES +=
FORMS +=

RESOURCES += \
    resource.qrc
