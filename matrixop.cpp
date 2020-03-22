#include "matrixop.h"
#include "Route_man.h"
#include "DijkstraForDis.h"

#include <fstream>
#include <QDebug>
#include <set>
#include <unordered_map>
#include <cstdio>


using namespace std;

MatrixOp::MatrixOp()
{

}

void MatrixOp::createMatrix(const char *filename) {
    qDebug() << filename << "-> A";
    ifstream fin(filename, ios_base::in);

    int cnt = 0; // 为了给站点名放置下标而设置的标识，每次加一

    //int num; // 站点数量
    fin >> num; //输入站点数量

    string station_name;

    for(int i = 0; i < num; i++) {
        //每个站点的scene 位置
        int x, y;
        fin >> station_name >> x >> y;
        poses.push_back(QPointF(x, y));
        num_to_name[cnt++] = station_name;
        name_to_num[station_name] = cnt-1;
    }

    fin >> route_num;

    Route_man<Route> route_man;

    set<int> stations;

    //如果这里要被分开成为一个函数，那么返回Dis *** 以及 传入fin
    //定义数组
    //int ***A = new int**[route_num];
    A = new Dis**[route_num];

    //读取数据，作一些初始化工作，对角线是0，没有连通是无穷大
    ////////////////////////////////////////////////////////////////////

    for(int i = 0; i < route_num; i++) {
        A[i] = new Dis*[num];

        for(int j = 0; j < num; j++) {
            A[i][j] = new Dis[num];
            for(int k = 0; k < num; k++) {
                A[i][j][k].value = INT_MAX;
            }
        }

        //初始化对角线
        for(int k = 0; k < num; k++)
            A[i][k][k].value = 0;

        // 读取路线编号
        int NoDot; // 写作No. 念作number
        fin >> NoDot;
        //03-07 原来之前我已经发现了这一点，也就是我今天在./file/TODO里提到的那个需求，为read.cpp加入一个读取编号的功能
        route_to_which_matrix[NoDot] = i;
        matrix_target_route[i] = NoDot;

        route_man.addRoute(NoDot);
        cout << route_man.debug_size() << "debug size" << endl;

        int route_len;
        fin >> route_len;
        string front_name, behind_name; //要实现更新节点，需要记录下前一个和后一个，然后用map获取需要更新的坐标
        //更新邻接矩阵
        //由于我们只有一个单向的路线，所以在用它来更新邻接矩阵的时候需要记下前一个，不然指针跳到下一个可就没办法知道前一个是什么了，从而没办法更新A[][]
        fin >> front_name;

        //routes[i].stations.push_back(name_to_num[front_name]);
        //replaced with route_man
        route_man[NoDot].addStop(name_to_num[front_name]);
        

        stations.insert(name_to_num[front_name]);

        for(int k = 1; k < route_len; k++) {
            fin >> behind_name; 
            
            //routes[i].stations.push_back(name_to_num[behind_name]);
            //replaced with route_man
            route_man[NoDot].addStop(name_to_num[behind_name]);

            stations.insert(name_to_num[behind_name]);
            
            A[i][name_to_num[front_name]][name_to_num[behind_name]].value = A[i][name_to_num[behind_name]][name_to_num[front_name]].value = 1;
            front_name = behind_name;
        }
    }

    fin.close();

    /*
    for(auto i : route_to_which_matrix) {
        cout << "route " << i.first << endl;
        for(auto j : route_man[i.first].stops) {
            cout << num_to_name[j] << endl;
        }
        cout << endl;
    }
    */

    //Dijkstra算法好像也要修改了
    //TODO
    //重写一个Dijkstra for Dis
    //
    //调用dijkstra算法，算出来每个矩阵的直达情况，然后给出距离
    for(int i = 0; i < route_num; i++) {
        Dijkstra(A[i], num, i);
    }

    for(int i = 0; i < route_num; i++) {
        print_num_matrix(A[i], num);
        printf("///////////////////////////////\n");
    }     

    for(int i = 0; i < route_num; i++) {
    //for(int i = 0; i < 1; i++) {
        cout << "route:" << i+1 << endl;
        print_matrix(A[i], num);
    }

    //输出路线信息，检查是否正确
    unordered_map<pair<int, int>, int, pair_hash> reach;
    for(int i = 0; i < route_num; i++) {
        for(int j = 0; j < num; j++) {
            for(int k = 0; k < num; k++) {
                if(A[i][j][k].value != 0 && A[i][j][k].value != INT_MAX) {
                    if(reach.count(pair<int,int>(j,k)) == 0 && reach.count(pair<int,int>(k,j)) == 0)
                            reach[pair<int, int>(j,k)] = A[i][j][k].value;
                }
            }
        }
        for(auto it = reach.begin(); it != reach.end(); it++) {
            cout << num_to_name[(it->first).first] << "---" << num_to_name[(it->first).second] << " with a distance " << it->second << endl;
        }
        
        //TODO
        //这边也得改
        print_matrix(A[i], num);
        reach.clear();
    }
    //改用新的输出路线的方式
    
    //参考论文里面，对所有A[i]，算出来一个min矩阵，也就是min[i][j] = min(A[k][i][j], k = 0, 1, 2, 3...
    //num是公交站的数量，在这里size和num说的是同一个东西
    Dis **B =  
        combineAs(route_num, num);

    Dijkstra(B, num, -1);
    cout << "after combination and Dijstra algorithm: \n";
    for(int i = 0; i < num; i++) {
        for(int j = 0; j < num; j++)
            if(B[i][j].value != INT_MAX)
                cout << B[i][j].value << ' ';
            else
                cout << "∞" << ' ';
        cout << endl;
    }

    D = B;
    
    print_matrix(D, num);
    cout << "print path num matrix" << endl;
    print_path_num_matrix(D, num);
}

MatrixOp::~MatrixOp() {
    // destruct
    for(int k = 0; k < route_num; k++) {
        for(int i = 0; i < num; i++) {
            delete[] A[k][i];
        }
        delete[] A[k];
    }
    delete[] A;

    for(int k = 0; k < num; k++) {
        delete[] D[k];
    }
    delete[] D;
}

Dis** MatrixOp::combineAs(int route_num, int size) {
    Dis **res = new Dis*[size];
    for(int i = 0; i < size; i++) {
        res[i] = new Dis[size];
    }

    for(int j = 0; j < size; j++)
        for(int k = 0; k < size; k++)
            res[j][k].value = INT_MAX;

    for(int j = 0; j < size; j++) {
        for(int k = 0; k < size; k++) {
            for(int i = 0; i < route_num; i++) {
                res[j][k] = A[i][j][k].value < res[j][k].value ? A[i][j][k] : res[j][k];
            }
            if(res[j][k].value != INT_MAX && res[j][k].value != 0) {
                res[j][k].value = 1;
            }
        }
    }
    return res;
}

//打印矩阵的信息
void MatrixOp::print_matrix(Dis **B, int num) {

    //这个矩阵是35 × 35，下标是[0-34][0-34]
    //直接输出出来

    for(int i = 0; i < num; i++) {
        //for(int j = 0; j < num; j++) {
        //更改为上半片区
        for(int j = i; j < num; j++) {
            if(B[i][j].value == INT_MAX || B[i][j].value == 0) continue;
            qDebug() << "position [" << stringOp.str2qstr(num_to_name[i]) << "][" << stringOp.str2qstr(num_to_name[j]) << "] :";
            qDebug() << "有" << B[i][j].path.size() << "条路径";
            for(auto onePath : B[i][j].path) {
                for(auto edge : onePath) {
                    //qDebug("%s (%d 号线) --> ", stringOp.str2qstr(num_to_name[edge.vex]), edge.route_num+1);
                    qDebug("%s (%d 号线) --> ", num_to_name[edge.vex].c_str(), edge.route_num+1);
                }
                qDebug() << "|";
            }
            //cout << "\n\n";
            qDebug() << " ";
        }
    }

}

void MatrixOp::print_num_matrix(Dis **a, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(a[i][j].value != INT_MAX)
                cout << a[i][j].value << ' ';
            else
                cout << "∞ ";
        }
        cout << endl;
    }
}

void MatrixOp::print_path_num_matrix(Dis **a, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            printf("%2ld ", a[i][j].path.size());
        }
        printf("\n");
    }
}
