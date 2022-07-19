QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpu.cpp \
    cpuwindow.cpp \
    draw.cpp \
    injectorwindow.cpp \
    instruction.cpp \
    main.cpp \
    mainwindow.cpp\
    memorywindow.cpp \
    mygraphicsscene.cpp\
    mygraphicsview.cpp \
    routerwindow.cpp

HEADERS += \
    cpu.h \
    cpuwindow.h \
    draw.h \
    global.h \
    injectorwindow.h \
    instruction.h \
    mainwindow.h \
    memorywindow.h \
    mygraphicsscene.h \
    mygraphicsview.h \
    routerwindow.h

FORMS += \
    cpuwindow.ui \
    injectorwindow.ui \
    mainwindow.ui \
    memorywindow.ui \
    routerwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
