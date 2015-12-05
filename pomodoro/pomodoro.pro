#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T17:44:29
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PomodoroTimer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    help.cpp

HEADERS  += mainwindow.h \
    help.h

FORMS    += mainwindow.ui \
    help.ui

RESOURCES += \
    resources.qrc
