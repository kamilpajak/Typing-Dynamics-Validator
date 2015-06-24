TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += validator.cpp

unix|win32: LIBS += -lmysqlcppconn
