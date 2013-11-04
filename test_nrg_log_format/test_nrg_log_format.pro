#-------------------------------------------------
#
# Project created by QtCreator 2011-01-08T20:23:59
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_nrg_log_format
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../../LogViewer
DEPENDPATH += $$PWD/../../LogViewer

SOURCES += tst_nrg_log_format.cpp\
    log_item.cpp \
    nrg_log_item.cpp \
    log_format.cpp \
    start_stop_buffer.cpp \
    log_column.cpp

HEADERS  += log_item.h \
    nrg_log_item.h \
    nrg_log_format.h \
    log_format.h \
    start_stop_buffer.h \
    log_column.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"

