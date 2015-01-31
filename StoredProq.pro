#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T00:31:26
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = StoredProq
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    operation.cpp

HEADERS += \
    operation.h \
    sqlmapper.h \
    queryresult.h \
    pg_types.h
CONFIG += c++11

