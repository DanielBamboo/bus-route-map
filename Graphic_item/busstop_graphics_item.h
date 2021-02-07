#ifndef BUS_H
#define BUS_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QString>

/*
* 地图上每个小公交站点对应的类
*/

class BusStop_Graphics_Item : public QObject,  public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    BusStop_Graphics_Item(QGraphicsItem *parent = nullptr);
    BusStop_Graphics_Item(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

    void setName(QString name);

    ~BusStop_Graphics_Item() {}
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
signals:
    // 当点击站点的时候发送一个信号说：我是什么名字
    void whoAmI(QString name);
private:
    QString name;
    int index;
};

#endif // BUS_H
