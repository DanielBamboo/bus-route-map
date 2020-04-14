#include "editroutes.h"
#include "ui_editroutes.h"
#include "search_box/searchbox.h"
#include <map>
#include <QDebug>
#include "stringop.h"
#include <QInputDialog>
#include <string>
#include <vector>
#include <QVector>
#include <QMessageBox>

EditRoutes::EditRoutes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditRoutes)
{
    ui->setupUi(this);
    ui->verticalLayout_2->sizeHint();

    //这段会出错是因为EditRoutes的setMap还没有完成
    /*
    SearchBox *lineEdit = new SearchBox();
    lineEdit->setMap(mapOp);
    lineEdits.push_back(lineEdit);
    ui->verticalLayout_2->addWidget(lineEdit);
    */

    ui->inputRouteNumberLineEdit->setValidator(new QIntValidator(0, 999, this));
    //添加限制条件ui->stop

    getConnections();
}

void EditRoutes::getConnections() {
    //用于添加线路的时候增加一个lineEdit
    connect(ui->addStopButton, &QPushButton::clicked, this, &EditRoutes::addLineEdit);

    //底部的三个按钮
    connect(ui->comfirmButton, &QPushButton::clicked, this, &EditRoutes::comfirm);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditRoutes::cancel);
    connect(ui->applyButton, &QPushButton::clicked, this, &EditRoutes::apply);

    //用于tab_1
    connect(ui->listRoutes, &QListWidget::currentRowChanged, this, &EditRoutes::showRoute);
    connect(ui->insertStopButton, &QPushButton::clicked, this, &EditRoutes::insertAStopForARoute);
    connect(ui->delStopButton, &QPushButton::clicked, this, &EditRoutes::delAStopFromARoute);

    //用于tab_2
    connect(ui->delRouteButton, &QPushButton::clicked, this, &EditRoutes::delARoute);

    //用于tab_3
    connect(ui->comfirmAddRouteButton, &QPushButton::clicked, this, &EditRoutes::addARoute);
    connect(ui->delStopButtonTab3, &QPushButton::clicked, this, &EditRoutes::popLineEdit);

    //用于tab_4
    connect(ui->createStopButton, &QPushButton::clicked, this, &EditRoutes::createAStop);
}

EditRoutes::~EditRoutes()
{
    qDebug("~EditRoutes()");
    for(auto &i : lineEdits) {
        delete i;
    }
    delete ui;
}

//还需要检测违规行为
void EditRoutes::addARoute() {
    //获取插入的路径编号
    int target_route_number = (ui->inputRouteNumberLineEdit->text()).toInt();
    qDebug() << "路径:" << target_route_number;

    //将输入的站点转换成为编号
    std::vector<int> newRouteStops;
    for(auto pi : lineEdits) {
        newRouteStops.push_back((mapOp->name_to_num[mapOp->stringOp.qstr2str((*pi).text())]));
    }

    //来输出的消息
    QString info_text = QString::number(target_route_number) + QString(" 号线: \n");
    //输出已经输入的这些路径
    for(int i = 0; i < lineEdits.size(); i++) {
        if(i == 0) {
            info_text += (*(lineEdits[i])).text();
        } else {
            info_text += " -> ";
            info_text += (*(lineEdits)[i]).text();
        }
        qDebug() << newRouteStops[i] << (*(lineEdits[i])).text();
    }

    if((mapOp->route_man).check_index_legal(target_route_number)) {
        QMessageBox::information(nullptr, "检测到违规动作", "输入的线路编号已经存在，请重新输入");
        ui->inputRouteNumberLineEdit->clear();
        return;
    }

    (mapOp->route_man).addRoute(target_route_number);
    (mapOp->route_man)[target_route_number].stops = newRouteStops;

    QMessageBox::information(nullptr, "添加成功", info_text);
}

void EditRoutes::delARoute() {
    QString cur_selected_route = ui->listRoutesTab2->item(ui->listRoutesTab2->currentRow())->text();
    QString route_name = cur_selected_route;

    cur_selected_route = cur_selected_route.mid(2);
    int target_route_num = cur_selected_route.toInt();
    (mapOp->route_man).del(target_route_num);
    int row = ui->listRoutesTab2->currentRow();
    ui->listRoutesTab2->takeItem(row);
    ui->listRoutes->takeItem(row);

    QMessageBox::information(nullptr, "完成信息", route_name+QString(" 已被删除"));
}

//还要处理异常情况，比如未选中任何条目要弹框警告
void EditRoutes::delAStopFromARoute() {
    qDebug("del stop from route");
    // 获取要删除的路径编号
    QString cur_selected_route = ui->listRoutes->item(ui->listRoutes->currentRow())->text();
    cur_selected_route = cur_selected_route.mid(2);
    int target_route_num = cur_selected_route.toInt();

    // 获得要删除的是第几个站点
    int target_stop_index = ui->listStops->currentRow();
    qDebug() << "删除的是" << target_route_num << "号线的" << target_stop_index;

    (mapOp->route_man)[target_route_num].delStop(target_stop_index + 1);
    ui->listStops->takeItem(target_stop_index);

}

void EditRoutes::insertAStopForARoute() {
    //有一个弹框，弹出来之后又是一个searchbox，获取要删除的站点之后删除
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("插入站点"), tr("请输入要插入的站点名称"), QLineEdit::Normal, 0, &ok);
    qDebug() << "ok : " << ok;
    if(ok && !text.isEmpty()) {
        //检查这个站点名称是否存在
        qDebug(" enter the if clause, okay");
        QVector<QString> names = mapOp->QgetNames();
        names.push_back(text);	//顺序查找监视哨
        int i;
        for(i = 0; i < names.size(); i++) {
            if(names[i] == text) {
                break;
            }
        }

        //检查是否重复了站点名称
        if(i < names.size() - 1) {
            qDebug() << text;
            qDebug() << ui->listRoutes->currentRow() << "(#listRoutes)";
            qDebug() << ui->listStops->currentRow() << "(#listStops)";
            if(ui->listRoutes->currentRow() == -1 || ui->listStops->currentRow() == -1) {
                QMessageBox::information(nullptr, "检测到违规动作", "请选择要插入位置");
            }

            //获取要插入站点的线路编号
            QString cur_selected_route = ui->listRoutes->item(ui->listRoutes->currentRow())->text();
            cur_selected_route = cur_selected_route.mid(2);
            int target_route_num = cur_selected_route.toInt();

            //获取从用户获取的text对应的站点编号
            int insert_stop_number = (mapOp->name_to_num)[(mapOp->stringOp).qstr2str(text)];
            qDebug() << "要插入的站点是 " << text << " 编号是 " << insert_stop_number;

            //检查text与这条线路的其他站点重复
            for(auto i : (mapOp->route_man)[target_route_num].stops) {
                if(i == insert_stop_number) {
                    QMessageBox::information(nullptr, "检测到违规动作", "你输入了一个已经存在于这条线路的站点");
                    return;
                }
            }

            //获取要插入的位置
            int target_stop_index = ui->listStops->currentRow();


            (mapOp->route_man)[target_route_num].insertStop(insert_stop_number, target_stop_index+1);

            //检查一下这条线路目前的情况
            for(auto i : (mapOp->route_man)[target_route_num].stops) {
                qDebug() << mapOp->stringOp.str2qstr(mapOp->num_to_name[i]);
            }

            ui->listStops->insertItem(target_stop_index, text);
        } else {
            qDebug("else");
            QMessageBox::information(NULL, "输入有误", "输入了一个不存在的站点名字");
        }

    }
}

void EditRoutes::fullListRoutes() {
    for(auto i : mapOp->matrix_target_route) {
        ui->listRoutes->insertItem(ui->listRoutes->count(), QString("线路")+QString::number(i.second));
        ui->listRoutesTab2->insertItem(ui->listRoutes->count(), QString("线路")+QString::number(i.second));
    }
}

void EditRoutes::showRoute() {
    QString cur_selected_route = ui->listRoutes->item(ui->listRoutes->currentRow())->text();
    cur_selected_route = cur_selected_route.mid(2);

    qDebug() << cur_selected_route;

    ui->listStops->clear();

    for(auto i : (mapOp->route_man)[cur_selected_route.toInt()].stops) {
        ui->listStops->insertItem(ui->listStops->count(), StringOp().str2qstr(mapOp->num_to_name[i]));
    }
}


void EditRoutes::setMap(MatrixOp *mapOp) {
    this->mapOp = mapOp;
    fullListRoutes();	//填充listRoutes这个listWidget

    SearchBox *lineEdit = new SearchBox();
    lineEdit->setMap(mapOp);
    lineEdits.push_back(lineEdit);
    ui->verticalLayout_2->addWidget(lineEdit);
}

void EditRoutes::addLineEdit() {
    if(lineEdits.size() == 0) {
        QMessageBox::information(nullptr, "检测到违规操作", "站点数量已经为0，不要再点击删除按钮");
        return;
    }
    SearchBox *lineEdit = new SearchBox();
    lineEdit->setMap(mapOp);
    lineEdits.push_back(lineEdit); //暂时储存
    ui->verticalLayout_2->addWidget(lineEdit);
}

void EditRoutes::popLineEdit() {
    SearchBox *lineEdit = lineEdits.back();
    lineEdits.pop_back();
    ui->verticalLayout_2->removeWidget(lineEdit);
    delete lineEdit;
}

void EditRoutes::comfirm() {
    qDebug("comfirm");
    //mapOp->writeToFile("..\\testGraphics\\testOutput.txt");
    mapOp->writeToFile("..\\testGraphics\\route.txt");

    //重新构建矩阵
    mapOp->createMatrix("..\\testGraphics\\route.txt");

    //重新填充界面上的数据
    emit comfirmChanges(true);

    accept();
}

void EditRoutes::cancel() {
    qDebug("cancel");
    close();
}

void EditRoutes::apply() {
    qDebug("apply");
    //mapOp->writeToFile("..\\testGraphics\\testOutput.txt");
    mapOp->writeToFile("..\\testGraphics\\route.txt");

    //重新构建矩阵
    mapOp->createMatrix("..\\testGraphics\\route.txt");

    //重新填充界面上的数据
    emit comfirmChanges(true);
}

void EditRoutes::createAStop() {
    //不去修改 mapOp->num 防止delete的时候按mapOp->num来delete崩溃
    static int num = mapOp->num;
    std::string name = mapOp->stringOp.qstr2str(ui->stopNameLineEdit->text());
    if(mapOp->name_to_num.count(name) != 0) {
        QMessageBox::information(nullptr, "检测到违规动作", "这个站点名称已经存在");
        ui->stopNameLineEdit->clear();
        return;
    }

    //检查工作交给qt
    qreal x = ui->stopXLineEdit->text().toDouble();
    qreal y = ui->stopYLineEdit->text().toDouble();

    //正式添加
    mapOp->poses.push_back(QPointF(x, y));
    mapOp->name_to_num[name] = num;
    mapOp->num_to_name[num] = name;
    num++;

    qDebug() << "新增站点 " << mapOp->stringOp.str2qstr(name) << " " << x << "," << y;

    QString info_text;
    info_text += mapOp->stringOp.str2qstr(name);
    info_text = info_text + " (" + QString::number(x) + "," + QString::number(y) + ")";

    QMessageBox::information(nullptr, "添加成功", info_text);
}
