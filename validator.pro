TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += validator.cpp \
    user.cpp

unix|win32: LIBS += -lmysqlcppconn

HEADERS += \
    user.h
