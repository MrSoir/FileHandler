QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

CONFIG += warn_off
#CONFIG += Wno-sign-compare

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT += gui

SOURCES += \
        main.cpp \
    fileinfobd.cpp \
    filemanager.cpp \
    fileselector.cpp \
    filesearcher.cpp \
    stringops.cpp \
    fibdviewer.cpp \
    filequeue.cpp \
    filewatcher.cpp \
    taskfolderelapser.cpp \
    taskshowhiddenfiles.cpp \
    filemimetypehandler.cpp \
    listfiles.cpp \
    exec.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    fileinfobd.h \
    filemanager.h \
    fileselector.h \
    filesearcher.h \
    stringops.h \
    fibdviewer.h \
    filequeue.h \
    filewatcher.h \
    taskfolderelapser.h \
    taskshowhiddenfiles.h \
    filemimetypehandler.h \
    listfiles.h \
    exec.h
