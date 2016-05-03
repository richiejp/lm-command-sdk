QT += core serialport
QT -= gui

CONFIG += c++11

TARGET = setpin
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    lm048.cpp \
    ../../../build/lm048lib.c \

HEADERS += \
    lm048.h \
    ../../../build/lm048lib.h
