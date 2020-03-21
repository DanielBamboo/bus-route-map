#ifndef BUS_H
#define BUS_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QString>

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
    void whoAmI(QString name);
private:
    QString name;
    int index;
};

#endif // BUS_H
