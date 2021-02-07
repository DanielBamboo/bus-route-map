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

#include "data_structure/setPath.h"
#include "data_structure/matrixop.h"
#include "data_structure/Route_man_bst.h"
#include "UI_design/info.h"
#include "UI_design/editroutes.h"
#include "UI_design/routepenman.h"
#include "Graphic_item/busstop_graphics_item.h"
#include "tools/stringop.h"

/*
 * 主界面类
 * 包含了主界面所有的信息
*/

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
    void receive_comfirm_changes_from_editRoutes(bool);
    void on_actionDoSomeQueryTriggered();
    void on_actinoDoSomeEditTriggered();

private slots:
    void choose_from_from_map(bool);
    void choose_to_from_map(bool);


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QAction *aboutQtAction;
    QAction *doSomeQuery;
    QAction *doSomeEdit;

    QGraphicsPixmapItem *map;
    //BusStop_Graphics_Item **busStop;
    QVector<BusStop_Graphics_Item *> busStop;

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

    // mapOperation
    MatrixOp * mapOp;
    StringOp *stringOp;

    Info * info;
    EditRoutes *editRoutes;

private:
    int res_num;					//查询结果线路的数量
    std::set<Path> *res_paths;		//保存结果那些路径的引用
    int change_value;				//换乘次数
    QGraphicsLineItem **res_lines;	//查询结果画在地图上的那些线

    void show_path_on_map(const Path &res);
    void repaintBusStop();

    RoutePen_man routePen_man;
};
#endif // MAINWINDOW_H
