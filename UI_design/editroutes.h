#ifndef EDITROUTES_H
#define EDITROUTES_H

#include <QDialog>
#include <QLineEdit>
#include <QVector>
#include "data_structure/matrixop.h"
#include "search_box/searchbox.h"

/*
 * 编辑线路的界面类
 * 是一个独立的窗口
 * 功能：
 * 1、增加/删除一条公交线路
 * 2、增加一个站点
 * 3、往路径中添加/删除一个站点
*/

namespace Ui {
class EditRoutes;
}

class EditRoutes : public QDialog
{
    Q_OBJECT
friend class MainWindow;
public:
    explicit EditRoutes(QWidget *parent = nullptr);
    ~EditRoutes();
    void setMap(MatrixOp *mapOp);

signals:
    void comfirmChanges(bool changed);

public slots:
    void addLineEdit();
    void popLineEdit();
    void comfirm();					//确认
    void cancel();					//取消
    void apply();					//应用
    void showRoute();
    void delAStopFromARoute();
    void insertAStopForARoute();
    void delARoute();
    void addARoute();
    void createAStop();

    // tab 5
    void comfirmEdit();
    void showEditInfo();

private:
    Ui::EditRoutes *ui;
    MatrixOp *mapOp;
    QVector<SearchBox *> lineEdits;
    void getConnections();
    void fullListRoutes();

    //每一次修改过后要进行刷新editRoutes这个窗口视图
    void fresh();
};

#endif // EDITROUTES_H
