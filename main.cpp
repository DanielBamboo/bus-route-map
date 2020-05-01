#include "mainwindow.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSplashScreen>
#include "customview.h"
#include "busstop_graphics_item.h"
#include <QPixmap>
#include <QTextCodec>

#include "search_box/pinyin.h"
#include <string>
#include <QString>
#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QApplication::addLibraryPath("./");
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    QPixmap pixmap(":/item/launch_image");
    QSplashScreen splash(pixmap);
    for(qint64 i = /*5555555*/20000000; i >= 0; i--) {
        splash.show();
    }


    /*
    std::string term = "晚安";
    ChineseConvert(term, term);
    std::cout << term << std::endl;


    QString term2 = "晚安";
    QString term3 = term2.at(0);
    qDebug() << term3;
    */

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
