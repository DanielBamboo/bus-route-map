#include "busstop_graphics_item.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

BusStop_Graphics_Item::BusStop_Graphics_Item(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent) {

}

BusStop_Graphics_Item::BusStop_Graphics_Item(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent) {

}

void BusStop_Graphics_Item::setName(QString name) {
    this->name = name;
}


void BusStop_Graphics_Item::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << name;
    emit whoAmI(name);
    event->accept();
}
