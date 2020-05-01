#include "routepenman.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>

RoutePen_man::RoutePen_man()
{
    colorSize = 5;
    colorArray = new QColor[colorSize];
    QColor a[10] = {Qt::black
                    , Qt::blue
                    , Qt::darkMagenta
                    , Qt::magenta
                    , Qt::yellow
                    , Qt::red
                    , Qt::darkYellow
                    , Qt::darkBlue
                    , Qt::darkCyan
                    , Qt::darkGreen
                   };
    QColor b[5] = {
        QColor(138, 198, 209),
        QColor(245, 88, 123),
        QColor(255, 181, 73),
        QColor(81, 91, 212),
        QColor(135, 226, 147)
    };
    for(int i = 0; i < colorSize; i++) {
        //colorArray[i] = a[i];
        colorArray[i] = b[i];
    }
    pen.setWidth(7);
    curColorIndex = 0;
    pen.setColor(colorArray[curColorIndex]);
}

QPen RoutePen_man::myPen() {
    return pen;
}

void RoutePen_man::nextColor() {
    qDebug("next color");
    qDebug() << "cur Color is" << curColorIndex;
    curColorIndex = (curColorIndex + 1) % colorSize;
    qDebug() << "next color is" << curColorIndex;
    pen.setColor(colorArray[curColorIndex]);
}

void RoutePen_man::setToRand0() {
    srand((unsigned int)time(NULL));
    curColorIndex = rand() % colorSize;
    pen.setColor(colorArray[curColorIndex]);
    qDebug() << "rand color begin is " << curColorIndex;
}
