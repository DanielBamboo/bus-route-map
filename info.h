#ifndef INFO_H
#define INFO_H

#include <QDialog>
#include <QVector>
#include <QString>
#include "matrixop.h"

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
