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

#include "customview.h"
#include "busstop_graphics_item.h"

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
    std::string qstr2str(const QString);
    QString str2qstr(const std::string);
    void showRowItem(int row);
    void readStops();
    void debug_printStops();

public slots:
    void deal_mousemove_signal(QPoint point);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void choose_from_from_map(bool);
    void choose_to_from_map(bool);

private:
    Ui::MainWindow *ui;
    QWidget *sceneWidget;
    QGraphicsScene *scene;
    CustomView *view;
    void createAction();
    void createMenu();

    QAction *aboutQtAction;

    QGraphicsPixmapItem *map;
    BusStop_Graphics_Item *busStop1;
    BusStop_Graphics_Item *busStop2;
    BusStop_Graphics_Item *busStop3;
    BusStop_Graphics_Item **busStop;
    int stop_num;

    Graphics_view_zoom *view_zoom;

    QMouseEvent *view_mouseEvent;

    std::vector<std::string> stops;
    //std::vector<std::pair<int, int>> poses;
    QVector<QPointF> poses;

    std::map<int, std::string> num_to_name;
    std::map<std::string, int> name_to_num;

    int choose_start_cnt;
    int choose_to_cnt;

};
#endif // MAINWINDOW_H
