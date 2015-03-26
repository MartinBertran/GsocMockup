#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T20:34:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtTuto
TEMPLATE = app

LIBS += -L/usr/include/openslide/ -lopenslide
INCLUDEPATH += /usr/include/openslide/

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ../../../usr/local/include/openslide/openslide.h \
    ../../../usr/local/include/openslide/openslide-features.h


FORMS    += mainwindow.ui

RESOURCES += \
    ../build-QtTuto-Desktop-Debug/textfinder.qrc
