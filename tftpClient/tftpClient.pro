#-------------------------------------------------
#
# Project created by QtCreator 2016-06-21T21:48:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tftpClient
TEMPLATE = app


SOURCES += main.cpp\
        tftpclientwindow.cpp \
    tftpdatagram.cpp \
    filehandler.cpp

HEADERS  += tftpclientwindow.h \
    tftpdatagram.h \
    filehandler.h

FORMS    += tftpclientwindow.ui
