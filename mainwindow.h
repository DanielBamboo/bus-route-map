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

#include "busstop_graphics_item.h"
#include "setPath.h"
#include "matrixop.h"
#include "stringop.h"

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
};
#endif // MAINWINDOW_H
