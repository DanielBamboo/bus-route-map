#ifndef MATRIXOP_H
#define MATRIXOP_H

#include <climits>
#include <map>
#include <cstring>
#include <string>
#include <QVector>
#include <QPointF>
#include <QPoint>

#include "setPath.h"
#include "stringop.h"

struct pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

// 对矩阵进行操作的一些类
class MatrixOp
{
friend class MainWindow;
public:
    MatrixOp();
    void readStops();
    void createMatrix(const char *filename);
    Dis** combineAs(int route_num, int size);

    void print_matrix(Dis **a, int size);
    void print_num_matrix(Dis **a, int size);
    void print_path_num_matrix(Dis **a, int size);

    ~MatrixOp();

private:
    std::map<std::string, int> name_to_num;
    std::map<int, std::string> num_to_name;

    //03-07 新建了两个全局变量，为了实现./file/TODO里的需求，即路径与矩阵的映射
    std::map<int, int> route_to_which_matrix;
    std::map<int, int> matrix_target_route;
    QVector<QPointF> poses; //POSES是按照编号储存的

private:
    int route_num;
    int num; //站点数量
    Dis ***A;
    Dis **D;

    StringOp stringOp;
};

#endif // MATRIXOP_H
