QT       += core gui
QT	 += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    busstop_graphics_item.cpp \
    customview.cpp \
    editroutes.cpp \
    graphics_view_zoom.cpp \
    info.cpp \
    main.cpp \
    mainwindow.cpp \
    matrixop.cpp \
    routepenman.cpp \
    search_box/googlesuggest.cpp \
    search_box/kmp.cpp \
    search_box/pinyin.cpp \
    search_box/searchbox.cpp \
    setPath.cpp \
    stringop.cpp

HEADERS += \
    Route_man_bst.h \
    busstop_graphics_item.h \
    customview.h \
    editroutes.h \
    graphics_view_zoom.h \
    info.h \
    mainwindow.h \
    matrixop.h \
    routepenman.h \
    search_box/googlesuggest.h \
    search_box/kmp.h \
    search_box/pinyin.h \
    search_box/searchbox.h \
    setPath.h \
    stringop.h

FORMS += \
    editroutes.ui \
    info.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

DISTFILES += \
    TODO \
    route.txt \
    stopPos.txt \
    stops.txt
