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
    Graphic_item/busstop_graphics_item.cpp \
    Graphic_item/graphics_view_zoom.cpp \
    UI_design/editroutes.cpp \
    UI_design/info.cpp \
    UI_design/routepenman.cpp \
    data_structure/matrixop.cpp \
    data_structure/setPath.cpp \
    search_box/googlesuggest.cpp \
    search_box/pinyin.cpp \
    search_box/searchbox.cpp \
    tools/stringop.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Graphic_item/busstop_graphics_item.h \
    Graphic_item/graphics_view_zoom.h \
    UI_design/info.h \
    UI_design/editroutes.h \
    UI_design/routepenman.h \
    data_structure/Route_man_bst.h \
    data_structure/setPath.h \
    data_structure/matrixop.h \
    search_box/googlesuggest.h \
    search_box/pinyin.h \
    search_box/searchbox.h \
    tools/stringop.h \
    mainwindow.h

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
