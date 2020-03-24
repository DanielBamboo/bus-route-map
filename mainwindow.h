#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QWidget>
#include <string>
#include <QEvent>
#include <vector>
#include <map>
#include <QVector>
#include <set>

#include "busstop_graphics_item.h"
#include "setPath.h"
#include "matrixop.h"
#include "stringop.h"
#include "Route_man.h"

class QPixmap;
class QGraphicsPixmapItem;
class Graphics_view_zoom;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showRoute();
    void showRowItem(int row);
    void debug_printStops();
    void createMap();
    void createOriginConnection();
    void createAction();
    void createMenu();

public slots:
    void deal_mousemove_signal(QPoint point);
    //bool eventFilter(QObject *target, QEvent *event);

private slots:
    void choose_from_from_map(bool);
    void choose_to_from_map(bool);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QAction *aboutQtAction;

    QGraphicsPixmapItem *map;
    BusStop_Graphics_Item **busStop;

    //站点数量
    int stop_num;

    //给graphicsView套的壳子
    Graphics_view_zoom *view_zoom;

    // ???
    QMouseEvent *view_mouseEvent;

    // 每次点击地图选点的时候就计数加一
    // 奇数开始选，偶数就停止选
    int choose_start_cnt;
    int choose_to_cnt;

    MatrixOp * mapOp;
    StringOp *stringOp;

//重新开一个域，用来存放查询之后级联操作所需要的东西
private:
    int res_num;					//查询结果线路的数量
    //或许我要再次使用route_man了 (暂时行不通)
    std::set<Path> *res_paths;		//保存结果那些路径的引用
    int change_value;				//换乘次数
    QGraphicsLineItem **res_lines;	//查询结果画在地图上的那些线

    void show_path_on_map(const Path &res);
};
#endif // MAINWINDOW_H
