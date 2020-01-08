TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lpcap

SOURCES += \
        capture.cpp \
        main.cpp \
        packet.cpp \
        statistics.cpp

HEADERS += \
    capture.h \
    packet.h \
    statistics.h
