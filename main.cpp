#include "mainwindow.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSplashScreen>
#include "Graphic_item/busstop_graphics_item.h"
#include <QPixmap>
#include <QTextCodec>

#include "search_box/pinyin.h"
#include <string>
#include <QString>
#include <QDebug>


/*
main 函数，开启qt的事件循环并且打开主窗口
*/

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QApplication::addLibraryPath("./");

    QPixmap pixmap(":/item/launch_image");
    QSplashScreen splash(pixmap);
    for(qint64 i = /*5555555*/20000000; i >= 0; i--) {
        splash.show();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
