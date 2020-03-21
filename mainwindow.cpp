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

#include <string>
#include <iostream>
#include <fstream>
#include "graphics_view_zoom.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , choose_start_cnt(0)
    , choose_to_cnt(0)
{
    readStops();
    debug_printStops();
    resize(1200,1200);
    //sceneWidget = new QWidget(this);
    //sceneWidget->setFixedSize(1196, 800);

    //view = new CustomView(sceneWidget);
    scene = new QGraphicsScene;
    ui->setupUi(this);

    view_zoom = new Graphics_view_zoom(ui->graphicsView);
    view_zoom->set_modifiers(Qt::NoModifier);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    map = new QGraphicsPixmapItem();
    map->setPixmap(QPixmap(":/map/map1196_1282"));

    /*
    QPointF pos1(151, 1097);
    QPointF pos2(593.104, 784.331);
    QPointF pos3(353, 1060);
    QPointF pos4(542, 1008);
    */

    /*
    QPointF *pos = new QPointF[stop_num];
    pos[0] = pos1;
    pos[1] = pos2;
    pos[2] = pos3;
    pos[3] = pos4;

    QString *name = new QString[stop_num];
    for(int i = 1; i <= stop_num; i++) {
        name[i-1] = "站点" + QString::number(i);
    }
    */

    busStop = new BusStop_Graphics_Item*[stop_num];
    for(int i = 0; i < stop_num; i++) {
        busStop[i] = new BusStop_Graphics_Item(map);
        busStop[i]->setPixmap(QPixmap(":/item/bus").scaled(20,20));
        busStop[i]->setPos(poses[i]-QPointF(10, 10));
        busStop[i]->setName(str2qstr(stops[i]));
    }

    /*
    busStop1 = new BusStop_Graphics_Item(map);
    busStop1->setPixmap(QPixmap(":/item/bus").scaled(20,20));
    busStop1->setPos(pos1);
    busStop1->setName("站点1");

    busStop2 = new BusStop_Graphics_Item(map);
    busStop2->setPixmap(QPixmap(":/item/bus").scaled(20,20));
    busStop2->setPos(pos2);
    busStop2->setName("站点2");


    busStop3 = new BusStop_Graphics_Item(map);
    busStop3->setPixmap(QPixmap(":/item/bus").scaled(20,20));
    busStop3->setPos(353-10, 1060-10);
    busStop3->setName("站点3");
    */

    /*
    QLineF routeLine1_2(pos1, pos2);
    QGraphicsLineItem *route1_2 = new QGraphicsLineItem(routeLine1_2, map);
    QPen pen;
    pen.setWidth(5);
    pen.setBrush(Qt::green);
    route1_2->setPen(pen);

    QLineF routeLine2(pos1, pos3);
    QGraphicsLineItem *route2 = new QGraphicsLineItem(routeLine2, map);
    route2->setPen(pen);

    QLineF routeLine3(pos4, pos3);
    QGraphicsLineItem *route3 = new QGraphicsLineItem(routeLine3, map);
    route3->setPen(pen);

    QLineF routeLine4(pos2, pos4);
    QGraphicsLineItem *route4 = new QGraphicsLineItem(routeLine4, map);
    route4->setPen(pen);
    */




    scene->addItem(map);

    //view->setScene(scene);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    //view->resize(1196, 800);
    //view->scene()->setSceneRect(0, 0,1149, 800);
    //this->setCentralWidget(sceneWidget);

    createAction();
    createMenu();

    QLabel *label = new QLabel(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::showRoute);

    //ui->routesList->insertItem(0, "1");
    //ui->routesList->insertItem(1, "1");

    connect(ui->routesList, &QListWidget::currentRowChanged, this, &MainWindow::showRowItem);

    ui->graphicsView->setMouseTracking(true);
    //ui->graphicsView->installEventFilter(this);
    connect(view_zoom, &Graphics_view_zoom::mousemove_signal, this, &MainWindow::deal_mousemove_signal);
    connect(ui->to_choose_button, &QPushButton::clicked, this, &MainWindow::choose_to_from_map);
    connect(ui->from_choose_button, &QPushButton::clicked, this, &MainWindow::choose_from_from_map);

    ui->textBrowser->setText("hello\nthisis\n|\nthis");

    for(int i = 0; i < stop_num; i++) {
        connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->label_4, &QLabel::setText);
    }
}

void MainWindow::readStops() {
    std::ifstream fin("..\\testGraphics\\stops.txt", std::ios_base::in);
    if(!fin.is_open()) {
        qDebug() << "打开失败";
        exit(-1);
    }
    int x;
    int y;
    std::string stop_name;

    int cnt;
    fin >> stop_num;

    for(int i = 0; i < stop_num; i++) {
        fin >> stop_name >> x >> y;
        num_to_name[i] = stop_name;
        name_to_num[stop_name] = i;

        stops.push_back(stop_name);
        poses.push_back(QPointF(x, y));
    }
}

void MainWindow::debug_printStops() {
    qDebug() << "打印中";
    for(int i = 0; i < stop_num; i++) {
        qDebug() << str2qstr(stops[i]) << ' ' << poses[i].x() << ' ' << poses[i].y();
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

std::string MainWindow::qstr2str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return std::string(cdata);
}

QString MainWindow::str2qstr(const std::string str)
{
    return QString::fromLocal8Bit(str.data());
}




//作为一个测试，要显示路线
void MainWindow::showRoute() {
    using namespace std;
    QString fr = ((QString)ui->lineEdit->text());
    QString to = ((QString)ui->lineEdit_2->text());
    string fr2 = qstr2str(fr);
    string to2 = qstr2str(to);
    //qDebug() << fr << to;
    cout << fr2 << '-' << to2 << endl;

    QString route = fr + "-" + to;

     //测试长地址
    for(int i = 0; i < 3; i++) {
        route = route + route;
    }

    ui->routesList->insertItem(ui->routesList->count(), route);
}

void MainWindow::showRowItem(int row) {
    QListWidgetItem *item = ui->routesList->item(row);
    std::cout << qstr2str(item->text()) << std::endl;
}


bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if(target == ui->graphicsView) {
        /*
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
        */
    }
}

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
        qDebug() << "开始选点";
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        }
        /*
        connect(busStop1, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        connect(busStop2, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        connect(busStop3, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        */
    } else {
        qDebug() << "结束选点";
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        }
        /*
        disconnect(busStop1, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        disconnect(busStop2, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        disconnect(busStop3, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        */
    }

}

void MainWindow::choose_to_from_map(bool) {
    choose_to_cnt++;
    if(choose_start_cnt % 2 == 1) {
        qDebug() << "开始选点";
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
        /*
        connect(busStop1, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        connect(busStop2, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        connect(busStop3, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        */
    } else {
        qDebug() << "结束选点";
        for(int i = 0; i < stop_num; i++) {
            connect(busStop[i], &BusStop_Graphics_Item::whoAmI, ui->lineEdit_2, &QLineEdit::setText);
        }
        /*
        disconnect(busStop1, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        disconnect(busStop2, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        disconnect(busStop3, &BusStop_Graphics_Item::whoAmI, ui->lineEdit, &QLineEdit::setText);
        */
    }
}
