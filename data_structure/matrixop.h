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
#include "tools/stringop.h"
#include "Route_man_bst.h"

/*
 * 核心类1
 * 整个程序的核心开始的地方
 * 名为MatrixOpration（矩阵操作类）
 * 它创建出来的对象我称作mapOp(mapOpration)，这个类即是以矩阵的形式保存数据，也是以矩阵的形式把
 * 地图展现出来
 *
 * 每一条公交线路对应着一个矩阵
 * 而所有n个站点又构成了一个n*n的矩阵，路径信息保存在n*n矩阵的元素里面
 * 在创建矩阵完毕之后所有的路径都会被计算出来
*/


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

class EditRoutes;
class Info;

// 对矩阵进行操作的一些类
class MatrixOp
{
friend class MainWindow;
friend class Info;
friend class EditRoutes;
public:
    MatrixOp();
    void readStops();
    void createMatrix(const char *filename);
    Dis** combineAs(int route_num, int size);

    void print_matrix(Dis **a, int size);
    void print_num_matrix(Dis **a, int size);
    void print_path_num_matrix(Dis **a, int size);

    QVector<QString> QgetNames();
    std::vector<std::string> getNames();


    void writeToFile(const char * filename);

    ~MatrixOp();

private:
    // 两个map
    // 一个是名字与数字的映射，一个是数字与名字的映射
    std::map<std::string, int> name_to_num;
    std::map<int, std::string> num_to_name;

    //03-07 新建了两个全局变量，为了实现./file/TODO里的需求，即路径与矩阵的映射
    std::map<int, int> route_to_which_matrix;			//这条线路对应着哪个矩阵
    std::map<int, int> matrix_target_route;				//这个矩阵对应着哪条线路
    QVector<QPointF> poses; //POSES是按照编号储存的

    void Dijkstra(Dis **arc, int size, int route_num);
    void Dijkstra(Dis **arc, int begin, int size, int route_num);

    void optimizeMatrix(Dis **arc, int size);

private:
    int route_num;
    int num; //站点数量

    Dis ***A;		//每一条线路对应着一个矩阵
    Dis **D;		//最终保存线路的矩阵

    StringOp stringOp;	//功能见stringop.h

    Route_man<Route *> route_man;
};

#endif // MATRIXOP_H
