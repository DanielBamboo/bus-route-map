#include "info.h"
#include "ui_info.h"

#include <QDebug>
//#include "Route_man.h"
#include "data_structure/Route_man_bst.h"
#include "tools/stringop.h"

Info::Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);

}

void Info::setMap(MatrixOp *mapOp) {
    this->mapOp = mapOp;
    /*
    for(auto i : mapOp->getNames()) {
        ui->comboBox->addItem(i);
    }
    */

    qDebug() << mapOp->matrix_target_route.size();

    for(auto i : mapOp->matrix_target_route) {
        ui->comboBox->addItem(QString("线路")+QString::number(i.second));
    }


    connect(ui->pushButton_2, &QPushButton::clicked, this, &Info::showRoutes);
    connect(ui->pushButton, &QPushButton::clicked, this, &Info::showStationInfo);
}

/*
显示整条路径的信息
比如选中1号线，就会输出1号线途径站点的序列
*/
void Info::showRoutes() {
    QString cur_selected_route = ui->comboBox->currentText();
    cur_selected_route = cur_selected_route.mid(2);

    qDebug() << cur_selected_route;

    ui->listWidget->clear();

    for(auto i : (mapOp->route_man)[cur_selected_route.toInt()]->stops) {
        ui->listWidget->insertItem(ui->listWidget->count(), StringOp().str2qstr(mapOp->num_to_name[i]));
    }
}

/*
输出经过某个站点的信息
比如输入 永定门 这个站点，会打印经过这个站点的所有公交线路
*/
void Info::showStationInfo() {
    QString inputStationName = ui->lineEdit->text();
    int stationNumber = mapOp->name_to_num[StringOp().qstr2str(inputStationName)];

    ui->listWidget_2->clear();

    for(int i = 0; i < mapOp->route_num; i++) {
        for(int j = 0; j < mapOp->num; j++) {
            if(mapOp->A[i][stationNumber][j].value != 0 && (mapOp->A)[i][stationNumber][j].value != INT_MAX) {
                ui->listWidget_2->insertItem(ui->listWidget_2->count(), QString::number(mapOp->matrix_target_route[i]) + QString("号线"));
                break;
            }
        }
    }
}

Info::~Info()
{
    delete ui;
}
