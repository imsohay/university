TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    formula.cpp \
    term.cpp \
    atomic_formula.cpp \
    calcformula.cpp \
    literal.cpp \
    interpretation.cpp \
    formula_wrapper.cpp

HEADERS += \
    formula.h \
    term.h \
    atomic_formula.h \
    calcformula.h \
    literal.h \
    interpretation.h \
    formula_wrapper.h

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
QMAKE_CXXFLAGS += -std=c++11
