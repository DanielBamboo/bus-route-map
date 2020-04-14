#include "mainwindow.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "customview.h"
#include "busstop_graphics_item.h"
#include <QPixmap>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::addLibraryPath("./");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    /*
    QGraphicsScene scene;
    //scene.addRect(QRectF(-10, -10, 20, 20));
    //scene.addText("hello");


    CustomView view(&scene);
    view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view.show();
    */

    MainWindow w;
    w.show();

    return a.exec();
}
