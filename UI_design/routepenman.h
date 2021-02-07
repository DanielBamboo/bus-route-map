#ifndef ROUTEPEN_H
#define ROUTEPEN_H

#include <QPen>
#include <QColor>

/*
 * 为了使线路五彩斑斓，创建了一个RoutePen_management类
 * 功能是实现打印不同公交线路的时候会自动切换画笔的颜色
 * 功能不简介了
*/

//一条线路对应一个颜色
class RoutePen_man
{
public:
    RoutePen_man();
    //根据线路编号设置颜色，也许是一个hash函数
    void nextColor();
    QPen myPen();
    void setToRand0();
    int colorSize;
private:
    int curColorIndex;
    QPen pen;

    QColor * colorArray;
};

#endif // ROUTEPEN_H
