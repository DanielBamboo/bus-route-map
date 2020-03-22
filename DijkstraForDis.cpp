#include <iostream>
#include <climits>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdio>

#include "./DijkstraForDis.h"
//#include "./Path.h"
//#include "newPath.h"
#include "setPath.h"

// TODO
// 还差等于的情况没写，当时因为出了bug所以注释掉了

using namespace std;

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

//TODO
//加入记录路径的功能

/*
int arc[6][6] = {
    0, INT_MAX, 1, 1, 1, INT_MAX,
    INT_MAX, 0, 1, INT_MAX, 1, 1,
    1, 1, 0, 1, 1, 1,
    1, INT_MAX, 1, 0, 1, INT_MAX,
    1, 1, 1, 1, 0, 1,
    INT_MAX, 1, 1, INT_MAX, 1, 0
};
*/
void Dijkstra(Dis **arc, int begin, int size, int route_num);

void Dijkstra(Dis **arc, int size, int route_num) {
    //我把下面dijkstra的原本用来初始化的路径的代码搬到这边来了,并且不再是和底下一样的初始化方式，即把原本value == 1的路径赋予一个初始短路径，不然在底下的assignPath()的时候会出现size = 0的情况。
    if(route_num != -1) {
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                if(arc[i][j].value == 1) {
                    Path onePath;
                    onePath.addEdge(i, route_num);
                    onePath.addEdge(j, route_num);
                    arc[i][j].addPath(onePath);
                }
            } 
        }

    } else {
        cout << "this dijkstra for D matrix" << endl;
    }
    cout << "bebore dijkstra route " << route_num << endl;
    for(int i = 0; i < size; i++) {
        Dijkstra(arc, i, size, route_num);
    }
}

void Dijkstra(Dis **arc, int begin, int size, int route_num) {
    //debug的标识
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

        /*
        cout << "min = " << min << " min_i = " << min_i << endl;
        */
        ++count;
        for(int i = 0; i < size; i++) {
            if(!visited[i] && arc[begin][min_i].value != INT_MAX && arc[min_i][i].value != INT_MAX) {

                if( arc[begin][i].value > arc[begin][min_i].value + arc[min_i][i].value ) {
                    arc[begin][i].value = arc[begin][min_i].value + arc[min_i][i].value;    
                    // 更改路径
                    arc[begin][i].assignPath( arc[begin][min_i].path * arc[min_i][i].path );
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

                    arc[begin][i].addPath(arc[begin][min_i].path * arc[min_i][i].path );


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
    for(int i = 0; i < size; i++) {
        //这个reverse是有一个范围的
        if(i == begin) continue;
        arc[i][begin].assignReversedPaths(arc[begin][i]);
        //arc[i][begin] = arc[begin][i];
    }
    delete[] visited; 
}

/*
int main() {
    cout << (arc[5][0] == INT_MAX) << endl;
    cout << "begin" << endl;
    for(int i = 0; i < size; i++) {
        Dijkstra(i);
        for(int i = 0; i<size; i++) {
            for(int j = 0; j < size; j++) {
                if(arc[i][j] == INT_MAX) {
                    cout << "W ";
                } else {
                    cout << arc[i][j] << ' ';
                }
            }
            cout << endl;
        }
        cout << "/////////////////////////" << endl;
    }
    return 0;
}
*/
