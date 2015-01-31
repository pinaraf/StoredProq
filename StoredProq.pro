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

SOURCES += src/main.cpp \
    src/operation.cpp

HEADERS += \
    src/operation.h \
    src/sqlmapper.h \
    src/queryresult.h \
    src/pg_types.h

CONFIG += c++11
