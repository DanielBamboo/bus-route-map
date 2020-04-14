#ifndef EDITROUTES_H
#define EDITROUTES_H

#include <QDialog>
#include <QLineEdit>
#include <QVector>
#include "matrixop.h"
#include "search_box/searchbox.h"

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
    void comfirm();
    void cancel();
    void apply();
    void showRoute();
    void delAStopFromARoute();
    void insertAStopForARoute();
    void delARoute();
    void addARoute();
    void createAStop();

private:
    Ui::EditRoutes *ui;
    MatrixOp *mapOp;
    QVector<SearchBox *> lineEdits;
    void getConnections();
    void fullListRoutes();
};

#endif // EDITROUTES_H
