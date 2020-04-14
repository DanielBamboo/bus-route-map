#include "matrixop.h"
#include "Route_man.h"
//#include "DijkstraForDis.h"

#include <fstream>
#include <QDebug>
#include <set>
#include <unordered_map>
#include <cstdio>
#include <climits>
#include <vector>


using namespace std;

MatrixOp::MatrixOp() :
    A(nullptr),
    D(nullptr),
    route_num(0),
    num(0)
{

}

void MatrixOp::createMatrix(const char *filename) {
    if(A != nullptr || D != nullptr) {
        qDebug("重新构造矩阵");
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

        num_to_name.clear();
        name_to_num.clear();
        route_to_which_matrix.clear();
        matrix_target_route.clear();
        poses.clear();
        route_man.clear();

        route_num = 0;
        num = 0;
    }

    qDebug() << filename << "-> A";
    ifstream fin(filename, ios_base::in);
    if(!fin.is_open()) {
        qDebug("in createMatrix:cannot open this file");
        exit(-2);
    }

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


    set<int> stations;

    //如果这里要被分开成为一个函数，那么返回Dis *** 以及 传入fin
    //定义数组
    //int ***A = new int**[route_num];
    A = new Dis**[route_num];

    //读取数据，作一些初始化工作，对角线是0，没有连通是无穷大
    ////////////////////////////////////////////////////////////////////

    for(int i = 0; i < route_num; i++) {
        A[i] = new Dis*[num];

        //初始化工作
        //初始化矩阵元素已经标记处所有的线路无法可达--INT_MAX
        for(int j = 0; j < num; j++) {
            A[i][j] = new Dis[num];
            for(int k = 0; k < num; k++) {
                A[i][j][k].value = INT_MAX;
            }
        }

        //初始化对角线元素为0
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
            
            A[i][name_to_num[front_name]][name_to_num[behind_name]].value
                    = A[i][name_to_num[behind_name]][name_to_num[front_name]].value
                    = 1;

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
        cout << "-------------------------------dealing " << i << " matrix---------------------------";
        Dijkstra(A[i], num, i);
    }

    /*
    for(int i = 0; i < route_num; i++) {
        print_num_matrix(A[i], num);
        printf("///////////////////////////////\n");
    }     

    for(int i = 0; i < route_num; i++) {
    //for(int i = 0; i < 1; i++) {
        cout << "route:" << i+1 << endl;
        print_matrix(A[i], num);
    }
    */

    //输出路线信息，检查是否正确
    //这是一个unordered_map，我写这一块代码主要是我想看看每个矩阵的情况是否良好
    //但是由于矩阵的对称性所以我需要用一个pair来防止重复
    /*
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
        //print_matrix(A[i], num);
        reach.clear();
    }
    //改用新的输出路线的方式
    */
    
    //参考论文里面，对所有A[i]，算出来一个min矩阵，也就是min[i][j] = min(A[k][i][j], k = 0, 1, 2, 3...
    //num是公交站的数量，在这里size和num说的是同一个东西
    Dis **B =  
        combineAs(route_num, num);

    cout << "after combination: \n";
    for(int i = 0; i < num; i++) {
        for(int j = 0; j < num; j++)
            if(B[i][j].value != INT_MAX)
                cout << B[i][j].value << ' ';
            else
                cout << "inf" << ' ';
        cout << endl;
    }

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
    
    /*
    print_matrix(D, num);
    */
    cout << "print path num matrix" << endl;

    print_path_num_matrix(D, num);
    fflush(stdout);
    qDebug("构造完成");
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

QVector<QString> MatrixOp::QgetNames() {
    QVector<QString> res;
    for(auto i : num_to_name) {
        res.push_back(stringOp.str2qstr(i.second));
    }
    return res;
}

std::vector<std::string> MatrixOp::getNames() {
    std::vector<std::string> res;
    for(auto i : num_to_name) {
        res.push_back(i.second);
    }
    return res;
}


void print_path(const set<Path> path) {
    int cnt = 0;
    for(auto i : path) {
        cout << "path" << ++cnt << ": ";
        for(auto j : i) {
            cout << j.vex << '(' << j.route_num << ") ";
        }
        cout << endl;
    }
}


void MatrixOp::Dijkstra(Dis **arc, int size, int route_num) {
    //我把下面dijkstra的原本用来初始化的路径的代码搬到这边来了,并且不再是和底下一样的初始化方式，即把原本value == 1的路径赋予一个初始短路径，不然在底下的assignPath()的时候会出现size = 0的情况。
    if(route_num != -1) {
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                if(arc[i][j].value == 1) {
                    Path onePath;
                    onePath.addEdge(i, matrix_target_route[route_num]);
                    onePath.addEdge(j, matrix_target_route[route_num]);
                    arc[i][j].addPath(onePath);
                }
            }
        }

    } else {
        cout << "this dijkstra for D matrix" << endl;
    }
    cout << "bebore dijkstra route " << route_num << endl;
    for(int i = 0; i < size; i++) {
        cout << "dijkstra i = " << i << " size = " << size << endl;
        Dijkstra(arc, i, size, route_num);
    }
}

void MatrixOp::Dijkstra(Dis **arc, int begin, int size, int route_num) {
    //debug的标识
    cout << "dijkstra multi-parameter" << endl;
    bool debug = false;
    //bool debug = true;

    /*
    if(route_num == -1){
        debug = true;
    }
    */

    int count = 0;
    //bool visited[size];
    bool *visited = new bool[size];
    memset(visited, 0, size*sizeof(bool));

    //wait, wait, 这个路径是不是可以用矩阵乘法来做？
    //回忆一下离散数学的内容
    //这个for应该是每个Matrix第一次初始化的时候调用的，后期combine之后再进行Dijkstra肯定不能像这样初始化了
    //所以是不是可以增加一个判断语句，即进入Dijkstra的条件，增加一个参数


    //怎么突然觉得初始化不可以在这个地方

    /*
    if(route_num != -1)
        for(int i = 0; i < size; i++) {
            if(!arc[begin][i].initialized) {
                Path initPath;
                initPath.addEdge(begin, route_num);
                initPath.addEdge(i, route_num);
                arc[begin][i].addPath(initPath);
                arc[begin][i].initialized = true;

                if(debug) {
                    printf("初始化一条边，form %d to %d\n", begin, i);
                }
            }
        }
    */

    while(count < size) {
        cout << "count is " << count << endl;
        int min_i = 0;
        int min = INT_MAX;
        for(int i = 0; i < size; i++) {
            if(!visited[i] && arc[begin][i].value < min) {
                min = arc[begin][i].value;
                min_i = i;
            }
        }
        visited[min_i] = true;


        cout << "min = " << min << " min_i = " << min_i << endl;
        if(min == INT_MAX) break;

        ++count;
        for(int i = 0; i < size; i++) {
            cout << "in for loop, i = " << i << endl;
            if(!visited[i] && arc[begin][min_i].value != INT_MAX && arc[min_i][i].value != INT_MAX) {
                cout << "#if = 1" << endl;
                if( arc[begin][i].value > arc[begin][min_i].value + arc[min_i][i].value ) {
                    arc[begin][i].value = arc[begin][min_i].value + arc[min_i][i].value;
                    // 更改路径

                    cout << "before loose -- 1" << endl;

                    arc[begin][i].assignPath( arc[begin][min_i].path * arc[min_i][i].path );

                    cout << "loose -- 1" << endl;

                    if(debug) {
                        printf("\n");
                        printf("assign 一条边 assign 之后path.size = %d\n", arc[begin][i].path.size());
                        printf("path lhs:\n");
                        print_path(arc[begin][min_i].path);
                        printf("path rhs:\n");
                        print_path(arc[min_i][i].path);
                        //printf("arc[%d][%d].value > arc[%d][%d].value + arc[%d][%d].value\n", begin, i, begin, min_i, min_i, i);

                        printf("arc[%d][%d].value(%d) > arc[%d][%d].value(%d) + arc[%d][%d].value(%d)\n", begin, i, arc[begin][i].value,  begin, min_i, arc[begin][min_i].value, min_i, i, arc[min_i][i].value);
                        print_path(arc[begin][i].path);
                        printf("\n");

                    }
                }
                //如果相等，那么就增加一条路径
                //好像有一个问题，会出现重复的path

                else if( arc[begin][i].value != 0 &&
                        arc[begin][min_i].value != 0 &&
                        arc[min_i][i].value != 0 &&
                        arc[begin][i].value == arc[begin][min_i].value + arc[min_i][i].value ) {

                    cout << "before loose -- 2" << endl;

                    arc[begin][i].addPath(arc[begin][min_i].path * arc[min_i][i].path );

                    cout << "loose -- 2" << endl;


                    if(debug) {
                        printf("\n == situation, addPath\n");
                        printf("path lhs:\n");
                        print_path(arc[begin][min_i].path);
                        printf("path rhs:\n");
                        print_path(arc[min_i][i].path);

                        printf("arc[%d][%d].value(%d) == arc[%d][%d].value(%d) + arc[%d][%d].value(%d)\n", begin, i, arc[begin][i].value,  begin, min_i, arc[begin][min_i].value, min_i, i, arc[min_i][i].value);
                        print_path(arc[begin][i].path);
                        printf("\n");
                    }
                }
                /*
                printf("update arc[%d][%d] = arc[%d][%d] + arc[%d][%d] \n", begin, i, begin, min_i, min_i, i);
                */

            }
        }
    }
    //update the colum
    cout << "update the colum" << endl;
    for(int i = 0; i < size; i++) {
        //这个reverse是有一个范围的
        if(i == begin) continue;
        arc[i][begin].assignReversedPaths(arc[begin][i]);
        //arc[i][begin] = arc[begin][i];
    }
    cout << "end update" << endl;
    delete[] visited;
}


void MatrixOp::writeToFile(const char * filename) {
    using namespace std;
    vector<string> names = getNames();
    ofstream fout(filename, ios_base::out | ios_base::trunc);
    if(fout.is_open() != true) {
        qDebug("wrong! cannot open this file");
        exit(-1);
    } else {
        qDebug("open successfully !");
    }

    if(names.size() != poses.size()) {
        qDebug("names's size is not the same sa poses's, wrong");
        exit(-2);
    }

    //要对这些names有筛选

    //暴力方式

    int i = 0;
    while(i < names.size()) {
        //检查names[i]是否在路径中
        int number = name_to_num[names[i]];
        //如果这个站点
        if(!route_man.check_exist_stop(number)) {
            names.erase(names.begin() + i);
            poses.erase(poses.begin() + i);
            continue;
        }
        i++;
    }

    fout << names.size() << endl;
    for(int i = 0; i < names.size(); i++) {
        fout << names[i] << "\t\t" << (int)poses[i].x() << " " << (int)poses[i].y() << endl;
    }

    fout << route_man.size() << endl;

    auto end = route_man.end();
    for(auto it = route_man.begin(); it != end; it++){
        fout << (*it).num << endl;
        fout << (*it).stops.size() << endl;
        for(auto index : (*it).stops) {
            fout << num_to_name[index] << endl;
        }
        fout << endl;
    }
}

