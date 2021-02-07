#include "matrixop.h"
#include "Route_man_bst.h"

#include <fstream>
#include <QDebug>
#include <set>
#include <unordered_map>
#include <cstdio>
#include <climits>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include "search_box/pinyin.h"

#define NORMAL_SIZ BUFSIZ
#define CHMSK_KEY 0xa5

using namespace std;

MatrixOp::MatrixOp() :
    A(nullptr),
    D(nullptr),
    route_num(0),
    num(0)
{

}
/*
 * 由于保存公交线路的文件已经被加密，所以这边会有一个decript函数用于解密文件，
*/
stringstream *decrypt(const char * filename)
{
    printf("normal_siz: %d\n", NORMAL_SIZ);
    FILE *fp = fopen(filename, "rb+");
    if(!fp) {
        qDebug() << "file do not open";
        exit(-2);
    }
    qDebug() << NORMAL_SIZ;
    int n;
    char *buf = new char[NORMAL_SIZ];
    char *data = new char[8196];
    data[0] = 0;
    int len = 0;
    qDebug("start to read\n");

    //如果已经加密，那么flag = 1; 否则为0
    int flag = 1;
    fread(buf, 1, 1, fp);
    if(buf[0] == '0') {
        qDebug("does not encrypt\n");
        flag = 0;
    } else if(buf[0] == '1'){
        qDebug("it is an encrypted file");
    } else {
        qDebug("error, file is not right");
        exit(-2);
    }
    buf[0] = 0;

    //while ((n = fread(buf, 1, NORMAL_SIZ, fp)) > 0)
    while(!feof(fp))
    {
        n = fread(buf, 1, NORMAL_SIZ, fp);

        qDebug() << "read" << n;

        if(flag == 1) {
            int i;
            for (i = 0; i < n; i++)
            {
                buf[i] ^= CHMSK_KEY;
            }
        }

        len += n;
        strcat(data, buf);
        data[len] = 0;

        buf[0] = 0;
    }
    data[len] = 0;
    //printf("len = %d\n", len);
    qDebug() << "len = " << len;

    //printf("%s\n", data);
    std::stringstream *s = new std::stringstream();
    *s << data;


    std::string t;

    /*
    qDebug() << "test decrypt";
    while(*s >> t) {
        //printf("%s ", t.c_str());
        qDebug() << StringOp::str2qstr(t);
    }
    */


    fflush(fp);

    fclose(fp);
    delete[] data;
    delete[] buf;
    printf("normal end");
    return s;
}

/*
 * 将路径信息加密之后保存在filename中
*/
int encrypt(const char * filename)
{
    FILE *fp = fopen(filename, "rb+");
    int n;
    char *buf = new char[NORMAL_SIZ];
    fread(buf, 1, 1, fp);
    if(buf[0] == '1') {
        printf("had encrypted\n");
        return -1; //表示已经加密
    } else {
        qDebug("not encrypted, start to encrypt");
    }
    buf[0] = 0;

    while ((n = fread(buf, 1, NORMAL_SIZ, fp)) > 0)
    {
        printf("read n: %d\n", n);
        int i;
        for (i = 0; i < n; i++)
        {
            buf[i] ^= CHMSK_KEY;
        }

        if (fseek(fp, -n, SEEK_CUR) == -1)
        {
            perror("fseek");
        }

        n = fwrite(buf, 1, n, fp);
        fflush(fp);//如果不加这句，那么文件大小如果小于库函数缓冲（默认是BUFSIZ大小）时实际上面的写操作并没写进去，从而导致下一次read还是成功，因此程序不会退出
        buf[0] = 0;
    }

    rewind(fp);
    fputc(49, fp); //填入1

    delete [] buf;

    fclose(fp);
    return 1;
}

/*
 * 核心函数：构造矩阵
 * 根据filename对应的文件来构造矩阵
 * 每一条线路对应着一个矩阵
 * 最后调用迪杰斯特拉算法计算出最后的D矩阵，它保存了所有站点之间的路径信息
*/
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

    //对这里进行修改
    /*
    ifstream fin(filename, ios_base::in);
    if(!fin.is_open()) {
        qDebug("in createMatrix:cannot open this file");
        exit(-2);
    }
    */

    std::stringstream *pfin = decrypt(filename);
    std::stringstream &fin = *pfin;

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
    qDebug() << "route num : " << route_num;


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
        //route_man[NoDot].addStop(name_to_num[front_name]);
        route_man[NoDot]->addStop(name_to_num[front_name]);


        stations.insert(name_to_num[front_name]);

        for(int k = 1; k < route_len; k++) {
            fin >> behind_name; 
            
            //routes[i].stations.push_back(name_to_num[behind_name]);
            //replaced with route_man
            //route_man[NoDot].addStop(name_to_num[behind_name]);
            route_man[NoDot]->addStop(name_to_num[behind_name]);

            stations.insert(name_to_num[behind_name]);
            
            A[i][name_to_num[front_name]][name_to_num[behind_name]].value
                    = A[i][name_to_num[behind_name]][name_to_num[front_name]].value
                    = 1;

            front_name = behind_name;
        }
    }

    //fin.close();
    delete pfin;


    /*
    for(auto i : route_to_which_matrix) {
        cout << "route " << i.first << endl;
        for(auto j : route_man[i.first].stops) {
            cout << num_to_name[j] << endl;
        }
        cout << endl;
    }
    */

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

    //对B调用Dijkstra算法，算出最后的矩阵
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

    print_path_num_matrix(D, num);

    // 优化路径，去除几乎重复的路径
    optimizeMatrix(D, this->num);
    
    /*
    print_matrix(D, num);
    */
    qDebug() << "print path num matrix" << endl;
    fflush(stdout);

    print_path_num_matrix(D, num);
    qDebug("构造完成");
    fflush(stdout);

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

/*
 * 对某个矩阵实现Dijkstra算法
 * 算法流程是对每一行依次使用迪杰斯特拉算法
 *
 * 参数：
 * arc			2d矩阵的指针
 * size			矩阵的大小
 * route_num	矩阵对应的线路编号，-1代表综合矩阵
*/
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

    //测试一下反向生成矩阵，结果是否一样，这里用随机数来决定生成顺序

    srand((unsigned int)time(NULL));
    int randTarget = rand();
    if(randTarget % 2 == 0) {
        //cout << "bebore dijkstra route " << route_num << endl;
        for(int i = 0; i < size; i++) {
            //cout << "dijkstra i = " << i << " size = " << size << endl;
            Dijkstra(arc, i, size, route_num);
        }
    } else {
        //cout << "bebore dijkstra route " << route_num << endl;
        for(int i = size-1; i >= 0; i--) {
            //cout << "dijkstra i = " << i << " size = " << size << endl;
            Dijkstra(arc, i, size, route_num);
        }
    }

    /*
    cout << "bebore dijkstra route " << route_num << endl;
    for(int i = 0; i < size; i++) {
        cout << "dijkstra i = " << i << " size = " << size << endl;
        Dijkstra(arc, i, size, route_num);
    }
    */
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

    while(count < size) {
        //cout << "count is " << count << endl;
        int min_i = 0;
        int min = INT_MAX;
        for(int i = 0; i < size; i++) {
            if(!visited[i] && arc[begin][i].value < min) {
                min = arc[begin][i].value;
                min_i = i;
            }
        }
        visited[min_i] = true;


        //cout << "min = " << min << " min_i = " << min_i << endl;
        if(min == INT_MAX) break;

        ++count;
        for(int i = 0; i < size; i++) {
            //cout << "in for loop, i = " << i << endl;
            if(!visited[i] && arc[begin][min_i].value != INT_MAX && arc[min_i][i].value != INT_MAX) {
                //cout << "#if = 1" << endl;
                if( arc[begin][i].value > arc[begin][min_i].value + arc[min_i][i].value ) {
                    arc[begin][i].value = arc[begin][min_i].value + arc[min_i][i].value;
                    // 更改路径

                    //cout << "before loose -- 1" << endl;

                    arc[begin][i].assignPath( arc[begin][min_i].path * arc[min_i][i].path );

                    //cout << "loose -- 1" << endl;

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

                    //cout << "before loose -- 2" << endl;

                    arc[begin][i].addPath(arc[begin][min_i].path * arc[min_i][i].path );

                    //cout << "loose -- 2" << endl;


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


/*
 *
*/

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

    //这是一个标志，表示未被加密
    fout << 0 << endl;

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

    //遍历所有的线路的站点
    /*
    auto end = route_man.end();
    for(auto it = route_man.begin(); it != end; it++){
        fout << (*it).num << endl;
        fout << (*it).stops.size() << endl;
        for(auto index : (*it).stops) {
            fout << num_to_name[index] << endl;
        }
        fout << endl;
    }
    */
    vector<int> keys = route_man.keys();
    for(auto i : keys) {
        fout << route_man.get(i)->num << endl;
        fout << route_man.get(i)->stops.size() << endl;
        vector<int> &stops = route_man.get(i)->stops;
        for(size_t j = 0; j < stops.size(); j++) {
            fout << num_to_name[stops[j]] << endl;
        }
        fout << endl;
    }
    encrypt(filename);
}

void printVector(vector<int> vec) {
    for(auto i : vec) {
        cout << i << ' ';
    }
    cout << endl;
}

// ugly way to optimize matrix
void MatrixOp::optimizeMatrix(Dis **arc, int size) {
    qDebug() << "do some test";
    std::set<int> c = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // erase all odd numbers from c
    for(auto it = c.begin(); it != c.end(); ) {
        if(*it % 2 == 1)
            it = c.erase(it);
        else
            ++it;
    }

    for(int n : c) {
        std::cout << n << ' ';
    }
    std::cout << endl;
    fflush(stdout);
    qDebug() << "optimizing matrix" << endl;

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            set<Path> * setPath =  &(arc[i][j].path);
            set<Path> newSetPath;
            set<vector<int>> existsJudger;

            for(auto i : *(setPath)) {
                Path path = i;
                path.calculateRoutes();
                //printVector(path.routes);
                if (existsJudger.count(path.routes) == 0) {
                    existsJudger.insert(path.routes);
                    newSetPath.insert(path);
                }
            }
            // begin to erase same
            /*
            qDebug() << "size of this set<Path> is " << newSetPath.size();
            qDebug() << "these are current routes in the setPath: ";
            for(auto i : newSetPath) {
                printVector(i.routes);
                fflush(stdout);
            }
            */

            //qDebug() << "assign to arc[i][j].path";
            fflush(stdout);

            arc[i][j].assignPath(newSetPath);
        }
    }
    qDebug() << "optimizing done" << endl;
}
