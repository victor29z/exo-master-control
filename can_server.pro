#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T10:40:23
#
#-------------------------------------------------

QT       += core gui serialbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = can_server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pcan_basic.cpp

HEADERS  += mainwindow.h \
    joint_data_type.h \
    pcan_basic.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/release/ -lpcan
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/debug/ -lpcan
else:unix: LIBS += -L$$PWD/../../../../usr/lib/ -lpcan

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include
