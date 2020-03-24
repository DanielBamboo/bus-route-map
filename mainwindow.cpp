#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QTextCodec>
#include <QDebug>
#include <QListWidget>
#include <QMouseEvent>
#include <QString>

#include <string>
#include <iostream>
#include <fstream>
#include "graphics_view_zoom.h"
#include "windows.h"

//变动：
//res_paths改指针
//为什么点两次查询会崩溃 (点两次查询并不会崩溃，而是点了查询再点listWeight再点查询会崩溃)
//因为在ui->routesList被删除的时候, 现在指向的row也会发生改变，改变row会调用一个槽，这个槽会获取当前row
//一开始我的解决方案是在showRoutes里里面先disconnect 然后 clear insert完成之后再connect
//现在的解决方案在row_change槽函数的首行

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , choose_start_cnt(0)
    , choose_to_cnt(0)
    , res_num(0)
    , change_value(0)
    , res_lines(nullptr)
{
    ui->setupUi(this);

    stringOp = new StringOp();

    const char * filename = "..\\testGraphics\\route.txt";

    mapOp = new MatrixOp();
    mapOp->createMatrix(filename);
    stop_num = mapOp->num;

    createMap();
    createAction();
    createMenu();
    createOriginConnection();
    //ui->textBrowser->setText("hello\nthisis\n|\nthis");

    /*
    int start = 5;
    int end = 7;
    qDebug() << (mapOp->D)[start][end].value;
    */
}

void MainWindow::createOriginConnection() {
    // 点击按钮开始生成路径
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::showRoute);

    //点击list里面的某一行（list是用于显示各种路径的），在终端显示路径
    connect(ui->routesList, &QListWidget::currentRowChanged, this, &MainWindow::showRowItem);

    // 在右上角显示坐标情况
    connect(view_zoom, &Graphics_view_zoom::mousemove_signal, this, &MainWindow::deal_mousemove_signal);

    //地图选点功能
    connect(ui->to_choose_button, &QPushButton::clicked, this, &MainWindow::choose_to_from_map);
    connect(ui->from_choose_button, &QPushButton::clicked, this, &MainWindow::choose_from_from_map);

    // 点击站点在右上角的label显示站点名
    for(int i = 0; i < stop_num; i++) {
        connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->show_stop_name_label, &QLabel::setText);
    }


}

void MainWindow::createMap() {
    //为GraphicsView套上可以缩放的壳子
    view_zoom = new Graphics_view_zoom(ui->graphicsView);
    view_zoom->set_modifiers(Qt::NoModifier);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    scene = new QGraphicsScene;

    map = new QGraphicsPixmapItem();
    map->setPixmap(QPixmap(":/map/map1196_1282"));

    scene->addItem(map);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setMouseTracking(true);

    // 创建公交站点

    busStop = new BusStop_Graphics_Item*[stop_num];
    for(int i = 0; i < stop_num; i++) {
        busStop[i] = new BusStop_Graphics_Item(map);
        busStop[i]->setPixmap(QPixmap(":/item/bus").scaled(20,20));
        busStop[i]->setPos(mapOp->poses[i] - QPointF(10, 10));
        busStop[i]->setName(stringOp->str2qstr(mapOp->num_to_name[i]));
    }
}

void MainWindow::debug_printStops() {
    qDebug() << "打印中";
    for(int i = 0; i < stop_num; i++) {
        qDebug() << stringOp->str2qstr(mapOp->num_to_name[i]) << ' ' << mapOp->poses[i].x() << ' ' << mapOp->poses[i].y();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createAction() {
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, &QAction::triggered, qApp, QApplication::aboutQt);
}

void MainWindow::createMenu() {
    QMenu *help = menuBar()->addMenu("&help");
    help->addAction(aboutQtAction);
}

//作为一个测试，要显示路线
void MainWindow::showRoute() {
    qDebug() << "showRoute";
    using namespace std;
    QString fr = ((QString)ui->lineEdit->text());
    QString to = ((QString)ui->lineEdit_2->text());
    string fr2 = stringOp->qstr2str(fr);
    string to2 = stringOp->qstr2str(to);

    int index_from = mapOp->name_to_num[fr2];
    int index_to = mapOp->name_to_num[to2];

    // 存一下结果路线集
    res_paths = &(mapOp->D[index_from][index_to].path);
    change_value = mapOp->D[index_from][index_to].value;
    qDebug() << "保存完成";
    //disconnect(ui->routesList, &QListWidget::currentRowChanged, this, &MainWindow::showRowItem);

    qDebug() << "当前row:" << ui->routesList->currentRow();

    ui->routesList->clear();
    qDebug() << "清除完成";
    for(int i = 0; i < (mapOp->D)[index_from][index_to].value; i++) {
        ui->routesList->insertItem(ui->routesList->count(), QString().asprintf("路线%d ", i+1));
    }
    qDebug() << "insertItems 完成";
    //connect(ui->routesList, &QListWidget::currentRowChanged, this, &MainWindow::showRowItem);

    /*
    ui->textBrowser->clear();
    for(auto onePath : mapOp->D[index_from][index_to].path) {
        for(auto edge : onePath) {
            //QString s;
            ui->textBrowser->append(stringOp->str2qstr(mapOp->num_to_name[edge.vex]) + QString().asprintf(" (%d 号线) -->\n", edge.route_num+1));
        }
    }
    */




    //cout << fr2 << '-' << to2 << endl;

    //QString route = fr + "-" + to;

    /*
     //测试长地址
    for(int i = 0; i < 3; i++) {
        route = route + route;
    }
    */

    //ui->routesList->insertItem(ui->routesList->count(), route);
}

void MainWindow::showRowItem(int row) {
    if(row == -1) return;
    QListWidgetItem *item = ui->routesList->item(row);
    qDebug() << item->text() << "row : " << row;

    //当row开始变化的时候，打印线路
    auto it = res_paths->begin();
    while(row--) {
        it++;
    }
    show_path_on_map(*it);
}


/*
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if(target == ui->graphicsView) {
        if(event->type() == QEvent::MouseMove) {

            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            QPoint point = mouseEvent->pos();
            std::cout << point.x() << '-' << point.y() << std::endl;
            //    显示view下的坐标
            ui->viewCoordLabel->setText(QString::asprintf("%d , %d",point.x(),point.y()));

            //    转换到Scene坐标
            QPointF pointScene=ui->graphicsView ->mapToScene(point);
            ui->sceneCoordLabel->setText(QString::asprintf("%.0f , %.0f", pointScene.x(),pointScene.y()));
            return true;
        }
    }
}
*/

void MainWindow::deal_mousemove_signal(QPoint point)
{
//    显示view下的坐标
    ui->viewCoordLabel->setText(QString::asprintf("%d , %d",point.x(),point.y()));

//    转换到Scene坐标
    QPointF pointScene=ui->graphicsView ->mapToScene(point);
    ui->sceneCoordLabel->setText(QString::asprintf("%.0f , %.0f", pointScene.x(),pointScene.y()));
}

void MainWindow::choose_from_from_map(bool) {
    choose_start_cnt++;
    if(choose_start_cnt % 2 == 1) {
        qDebug() << "开始选点from";
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        }
    } else {
        qDebug() << "结束选点from";
        for(int i = 0; i < stop_num; i++) {
            disconnect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        }
    }
}

void MainWindow::choose_to_from_map(bool) {
    static QGraphicsLineItem *lineItem;
    choose_to_cnt++;
    if(choose_to_cnt % 2 == 1) {
        QLineF line(10, 10, 500, 500);
        lineItem = new QGraphicsLineItem(line, map);
        qDebug() << "开始选点to stop_num = " << stop_num;
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
    } else {
        qDebug() << "结束选点to";
        delete lineItem;
        for(int i = 0; i < stop_num; i++) {
            disconnect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
    }
}


void MainWindow::show_path_on_map(const Path &res) {
    for(auto i : res) {
        qDebug() << stringOp->str2qstr(mapOp->num_to_name[i.vex]) << '(' << i.route_num << ')' << "-->";
    }
    std::cout << std::endl;

    static int lines_num = -1;
    //先把当前的res_lines都清除了
    //当然还得先在constructor里面把res_lines 初始化为 nullptr
    if(res_lines != nullptr) {
        qDebug() << "while delete";
        qDebug() << "lines_num :" << lines_num;
        for(int i = 0; i < lines_num; i++) {
            delete res_lines[i];
        }
        delete[] res_lines;
    }

    lines_num = res.len() - change_value; //有这么多线要画
    qDebug() << "lines_num " << lines_num;
    qDebug() << "res.len " << res.len();
    qDebug() << "change_value" << change_value;
    res_lines = new QGraphicsLineItem * [lines_num];

    int j = 0; // j 保证小于 res.len()，用来遍历这个Path的结点们

    for(int i = 0; i < lines_num && j < res.len(); i++, j++) {
        if(res.get(j).vex == res.get(j+1).vex) {
            j = j+1;
            if(j >= res.len()) {
                qDebug() << "res len out of bounds";
                exit(0);
            }
        }
        qDebug() << "connect " << stringOp->str2qstr(mapOp->num_to_name[res.get(j).vex]) << "and" << stringOp->str2qstr(mapOp->num_to_name[res.get(j+1).vex]);
        QLineF tmpLine(mapOp->poses[res.get(j).vex], mapOp->poses[res.get(j+1).vex]);
        res_lines[i] = new QGraphicsLineItem(tmpLine, map);
    }
}
