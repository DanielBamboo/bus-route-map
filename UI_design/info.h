#ifndef INFO_H
#define INFO_H

#include <QDialog>
#include <QVector>
#include <QString>
#include "data_structure/matrixop.h"

/*
 * 界面类，用于
 * 1、查询公交途经信息
 * 2、查询经过某个站点的所有公交的信息
*/

namespace Ui {
class Info;
}

class Info : public QDialog
{
    Q_OBJECT
friend class MainWindow;
public:
    explicit Info(QWidget *parent = nullptr);
    void setMap(MatrixOp *mapOp);
    void getConnections();
    ~Info();

public slots:
    void showRoutes();
    void showStationInfo();

private:
    Ui::Info *ui;
    MatrixOp *mapOp;
    QVector<QString> stationNameList;
};

#endif // INFO_H
