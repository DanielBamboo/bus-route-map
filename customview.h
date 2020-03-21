#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QGraphicsView>

class QWheelEvent;
class QWidget;
class QGraphicsScene;

class CustomView : public QGraphicsView
{
public:
    CustomView(QGraphicsScene *scene, QWidget *parent = nullptr);
    CustomView(QWidget *parent = 0) : QGraphicsView(parent) {}
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CUSTOMVIEW_H
