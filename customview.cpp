#include "customview.h"
#include <QWheelEvent>
#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QDebug>

CustomView::CustomView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
}

void CustomView::wheelEvent(QWheelEvent *event)
{
    // 获取当前鼠标相对于view的位置;
    QPointF cursorPoint = event->pos();
    qDebug() << "鼠标的位置相对于view:" << cursorPoint.x() << ' ' << cursorPoint.y();
    // 获取当前鼠标相对于scene的位置;
    QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
    qDebug() << "鼠标的位置相对于scene:" << scenePos.x() << ' ' << scenePos.y();

    // 获取view的宽高;
    qreal viewWidth = this->viewport()->width();
    qreal viewHeight = this->viewport()->height();

    // 获取当前鼠标位置相当于view大小的横纵比例;
    qreal hScale = cursorPoint.x() / viewWidth;
    qreal vScale = cursorPoint.y() / viewHeight;

    // 当前放缩倍数;
    qreal scaleFactor = this->matrix().m11();
    qDebug() << "scaleFactor: " << scaleFactor;
    int wheelDeltaValue = event->delta();
    if(scaleFactor < 1.1 && wheelDeltaValue < 0) return;
    if(scaleFactor > 2.5 && wheelDeltaValue > 0) return;
    // 向上滚动，放大;
    if (wheelDeltaValue > 0)
    {
        this->scale(1.2, 1.2);
    }
    // 向下滚动，缩小;
    else
    {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }

    // 将scene坐标转换为放大缩小后的坐标;
    QPointF viewPoint = this->matrix().map(scenePos);
    qDebug() << viewPoint.x() << ' ' << viewPoint.y();
    // 通过滚动条控制view放大缩小后的展示scene的位置;
    horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * hScale));
    verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * vScale));
    qDebug() << viewPoint.x() - viewWidth * hScale << "-" << viewPoint.y() - viewHeight * vScale;
}
/*
void CustomView::wheelEvent(QWheelEvent *event)
{
    // 当前放缩倍数;
    qreal scaleFactor = this->matrix().m11();
    int wheelDeltaValue = event->delta();
    // 向上滚动，放大;
    if (wheelDeltaValue > 0)
    {
        this->scale(1.2, 1.2);
    }
    // 向下滚动，缩小;
    else
    {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }
}
*/

void CustomView::mouseMoveEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->x(), event->y());
    qDebug() << "this from scene is (" << scenePos.x() << ", " << scenePos.y() << ")";
}
