TEMPLATE = lib
TARGET = AdvancedViews
QT += qml quick
CONFIG += plugin c++1z

TARGET = $$qtLibraryTarget($$TARGET)
uri = AdvancedViews

# Input
SOURCES += \
    advancedviews_plugin.cpp \
    range.cpp \
    axis.cpp \
    tableviewprivate.cpp

HEADERS += \
    advancedviews_plugin.h \
    range.h \
    axis.h \
    cell.h \
    table.h \
    tableviewprivate.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

RESOURCES += \
    resources.qrc