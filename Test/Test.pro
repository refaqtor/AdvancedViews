QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++1z
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \ 
    tst_advancedviews.cpp

INCLUDEPATH += ../AdvancedViews
