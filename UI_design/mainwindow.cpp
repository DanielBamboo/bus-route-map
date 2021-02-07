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

/*
主窗口的构造函数
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , choose_start_cnt(0)
    , choose_to_cnt(0)
    , res_num(0)
    , change_value(0)
    , res_lines(nullptr)
{
    //设置ui的显示
    ui->setupUi(this);

    stringOp = new StringOp();

    //路径信息的保存地点
    const char * filename = "..\\testGraphics\\route.txt";

    //创建核心类：地图类，地图类又叫矩阵类，路径信息被保存在了矩阵内
    mapOp = new MatrixOp();
    mapOp->createMatrix(filename);
    stop_num = mapOp->num;

    createMap();

    //接下来是qt中构造一些小部件
    ui->lineEdit->setMap(mapOp);
    ui->lineEdit_2->setMap(mapOp);

    info = new Info();
    info->setMap(mapOp);

    editRoutes = new EditRoutes();
    editRoutes->setMap(mapOp);

    //创建一些按钮与一些函数之间的绑定关系
    createOriginConnection();
    //创建菜单
    createAction();
    createMenu();
}

/*
创建链接函数
绑定按钮和对应的函数
按下按钮->调用函数
*/
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

    // 与EditRoutes的信号连接起来，更改数据的时候重新绘制站点
    connect(editRoutes, &EditRoutes::comfirmChanges, this, &MainWindow::receive_comfirm_changes_from_editRoutes);
}

/*
创建地图
*/
void MainWindow::createMap() {
    view_zoom = new Graphics_view_zoom(ui->graphicsView);
    view_zoom->set_modifiers(Qt::NoModifier);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    scene = new QGraphicsScene;

    //读取背景图片
    map = new QGraphicsPixmapItem();
    map->setPixmap(QPixmap(":/map/map1196_1282"));

    scene->addItem(map);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setMouseTracking(true);

    // 在地图上创建公交站点
    for(int i = 0; i < stop_num; i++) {
        BusStop_Graphics_Item *aStop = new BusStop_Graphics_Item(map);
        this->busStop.push_back(aStop);
        busStop[i]->setPixmap(QPixmap(":/item/bus").scaled(20,20));
        busStop[i]->setPos(mapOp->poses[i] - QPointF(10, 10));
        busStop[i]->setName(stringOp->str2qstr(mapOp->num_to_name[i]));
    }
}

/*
修改过路径信息之后要对公交站点的情况进行重新绘制
*/
void MainWindow::receive_comfirm_changes_from_editRoutes(bool changed) {
    qDebug("repaint");
    repaintBusStop();
}

/*
与上面的函数向对应
*/
void MainWindow::repaintBusStop() {
    for(int i = 0; i < busStop.size(); i++) {
        delete busStop[i];
    }
    busStop.clear();
    this->stop_num = mapOp->num;

    for(int i = 0; i < stop_num; i++) {
        BusStop_Graphics_Item *new_stop = new BusStop_Graphics_Item(map);
        this->busStop.push_back(new_stop);
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

/*
创建菜单项目
*/
void MainWindow::createAction() {
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, &QAction::triggered, qApp, QApplication::aboutQt);

    doSomeQuery = new QAction(tr("查询线路信息或站点信息"));
    connect(doSomeQuery, &QAction::triggered, this, &MainWindow::on_actionDoSomeQueryTriggered);
    doSomeEdit = new QAction(tr("更改一些信息"));
    connect(doSomeEdit, &QAction::triggered, this, &MainWindow::on_actinoDoSomeEditTriggered);
}

void MainWindow::on_actionDoSomeQueryTriggered() {
    info->show();
}

void MainWindow::on_actinoDoSomeEditTriggered() {
    editRoutes->show();
}

void MainWindow::createMenu() {
    QMenu *help = menuBar()->addMenu("&help");
    help->addAction(aboutQtAction);

    QMenu *tools = menuBar()->addMenu("&tools");
    tools->addAction(doSomeQuery);
    tools->addAction(doSomeEdit);
}

/*
从matrixOp类中读取出路径信息并且展示在地图界面中
*/
void MainWindow::showRoute() {
    qDebug() << "showRoute";
    using namespace std;
    // 获取输入的起点和终点
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

    qDebug() << "当前row:" << ui->routesList->currentRow();

    ui->routesList->clear();
    qDebug() << "清除完成";

    // 接下来的这一大段都是展示出来的信息
    // 信息被打印在一个框中，这个框叫textBrowser，以下是不断往这个框里添加信息
    ui->textBrowser->clear();
    for(auto i : *(res_paths)) {
        QString routeChangeInfo;
        QString infoForTextBrowser;
        routeChangeInfo += (QString::number(i.get(0).route_num) + "号线" + " "/*QString::number((*(res_paths)).size())*/);
        ui->textBrowser->append( mapOp->stringOp.str2qstr(mapOp->num_to_name[i.get(0).vex]) + " " + QString::number(i.get(0).route_num) + "号线" );
        for(int j = 1; j < i.len(); j++) {
            int changeFlag = 0;
            if(i.get(j).route_num != i.get(j-1).route_num) {
                routeChangeInfo += (" -> " + QString::number(i.get(j).route_num) + "号线\n");
                changeFlag = 1;
            }
            if(changeFlag) {
                ui->textBrowser->append("\n" + mapOp->stringOp.str2qstr(mapOp->num_to_name[i.get(j).vex]) + " " + QString::number(i.get(j).route_num) + "号线\n");
            } else {
                ui->textBrowser->append(mapOp->stringOp.str2qstr(mapOp->num_to_name[i.get(j).vex]) + "\n");
            }
        }
        ui->routesList->insertItem(ui->routesList->count(), routeChangeInfo);
    }
    qDebug() << "insertItems 完成";

    // 调用展示路径的函数
    show_path_on_map(*(res_paths->begin()));
}

/*
查询结果会有多种路线可以选择
当切换选中的路径的时候在地图上也要体现出来
显示出选中的路径
*/
void MainWindow::showRowItem(int row) {
    if(row == -1) return;

    //获取是要显示的是哪条路径
    QListWidgetItem *item = ui->routesList->item(row);
    qDebug() << item->text() << "row : " << row;

    //当row开始变化的时候，打印线路
    auto it = res_paths->begin();
    while(row--) {
        qDebug("it++");
        it++;
    }


    // 这一段代码和前面一个函数的最后面是一样的，出于时间关系没有合并成一个函数
    ui->textBrowser->clear();
    QString infoForTextBrowser;
    ui->textBrowser->append( mapOp->stringOp.str2qstr(mapOp->num_to_name[(*it).get(0).vex]) + " " + QString::number((*it).get(0).route_num) + "号线" );
    for(int j = 1; j < (*it).len(); j++) {
        int changeFlag = 0;
        if((*it).get(j).route_num != (*it).get(j-1).route_num) {
            changeFlag = 1;
        }
        if(changeFlag) {
            ui->textBrowser->append("\n" + mapOp->stringOp.str2qstr(mapOp->num_to_name[(*it).get(j).vex]) + " " + QString::number((*it).get(j).route_num) + "号线");
        } else {
            ui->textBrowser->append(mapOp->stringOp.str2qstr(mapOp->num_to_name[(*it).get(j).vex]));
        }
    }

    // 调用展示路径的函数
    show_path_on_map(*it);
}

void MainWindow::deal_mousemove_signal(QPoint point)
{
//    显示view下的坐标
    ui->viewCoordLabel->setText(QString::asprintf("%d , %d",point.x(),point.y()));

//    转换到Scene坐标
    QPointF pointScene=ui->graphicsView ->mapToScene(point);
    ui->sceneCoordLabel->setText(QString::asprintf("%.0f , %.0f", pointScene.x(),pointScene.y()));
}

/*
地图选点功能的槽函数
选择起点
*/
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

/*
选择终点
*/
void MainWindow::choose_to_from_map(bool) {
    choose_to_cnt++;
    if(choose_to_cnt % 2 == 1) {
        QLineF line(10, 10, 500, 500);
        qDebug() << "开始选点to stop_num = " << stop_num;
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
    } else {
        qDebug() << "结束选点to";
        for(int i = 0; i < stop_num; i++) {
            disconnect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
    }
}

/*
在地图上展示路径所用到的函数
*/
void MainWindow::show_path_on_map(const Path &res) {
    qDebug("show path on map");
    qDebug() << "res.length = " <<  QString::number(res.len());
    for(auto i : res) {
        qDebug() << stringOp->str2qstr(mapOp->num_to_name[i.vex]) << '(' << i.route_num << ')' << "-->";
    }
    std::cout << std::endl;


    static int lines_num = -1;
    //先把当前的res_lines都清除了
    //当然还得先在constructor里面把res_lines 初始化为 nullptr

    // 在打印新的线路之前先将之前的线路清空
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

    routePen_man.setToRand0();

    // 正式打印线路
    for(int i = 0; i < lines_num && j < res.len(); i++, j++) {
        if(res.get(j).vex == res.get(j+1).vex) {
            routePen_man.nextColor();
            j = j+1;
            if(j >= res.len()) {
                qDebug() << "res len out of bounds";
                exit(0);
            }
        }
        qDebug() << "connect " << stringOp->str2qstr(mapOp->num_to_name[res.get(j).vex]) << "and" << stringOp->str2qstr(mapOp->num_to_name[res.get(j+1).vex]);
        QLineF tmpLine(mapOp->poses[res.get(j).vex], mapOp->poses[res.get(j+1).vex]);
        res_lines[i] = new QGraphicsLineItem(tmpLine, map);
        res_lines[i]->setPen(routePen_man.myPen());
    }
}
