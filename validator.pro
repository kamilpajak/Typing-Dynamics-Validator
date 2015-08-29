TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += validator.cpp \
    user.cpp \
    sample.cpp \
    keystroke.cpp \
    profile.cpp \
    distance.cpp \
    threshold.cpp

unix|win32: LIBS += -lmysqlcppconn

HEADERS += \
    user.h \
    sample.h \
    keystroke.h \
    profile.h \
    distance.h \
    threshold.h
