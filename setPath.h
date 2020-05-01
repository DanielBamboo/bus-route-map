// TODO
// 可以造一个映射，因为矩阵有差不多一半的空间被浪费了
// 这个映射可以是一个类
//
// 关于为什么要用数组我想说没想到什么好的数据结构可以方便地复制和删除并且占用空间小
// 换用vector了
// addEdge用可变参数?
// 变为setPath
#ifndef __SET_PATH_H__
#define __SET_PATH_H__

#include <vector>
#include <set>
#include <iostream>
#include <climits>
#include <algorithm>

using std::set;
using std::cout;
using std::endl;
using std::vector;

//我在设计这个Edge的时候...实际上它是一个Vex
struct Edge {
    int route_num;	// 这个点归属于哪条线路
    int vex;		// 这个点的编号

    Edge(int vex, int route_num) {
        this->vex = vex;
        this->route_num = route_num;
    }
    bool operator == (const Edge &rhs) {
        if(this->route_num != rhs.route_num) 	return false;
        if(this->vex != rhs.vex)				return false;
        return true;
    }
};

class Path {
public:
    ~Path() {
        edge.clear();
    }
    // TODO
    // 这个加号得改，也就是删除的是lhs的末尾还是rhs的开头
    const Path operator +(const Path & rhs) const {
        Path tmp(*this);
        if(tmp.edge.back().vex == rhs.edge.front().vex) {
            if(tmp.edge.back().route_num == rhs.edge.front().route_num) {
                copy(rhs.edge.begin()+1, rhs.edge.end(), back_inserter(tmp.edge));
            } else {
                copy(rhs.edge.begin(), rhs.edge.end(), back_inserter(tmp.edge));
            }
        }
        return tmp;
    }

    bool operator < (const Path &rhs) const  {
        if(this->len() == rhs.len()) {
            for(int i = 0; i < this->len(); i++) {
                if(this->edge[i].vex != rhs.edge[i].vex) {
                    return this->edge[i].vex < rhs.edge[i].vex;
                } else if(this->edge[i].route_num != rhs.edge[i].route_num){
                    return this->edge[i].route_num < rhs.edge[i].route_num;
                }
            }
        } else {
            return this->len() < rhs.len();
        }
    }

    void addEdge(int vex, int route_num) {
        edge.push_back(Edge(vex, route_num));
    }

    vector<Edge>::const_iterator begin() const {
        return edge.cbegin();
    }
    
    vector<Edge>::const_iterator end() const {
        return edge.cend();
    }

    
    Path reversePath() {
        Path reverPath;
        vector<Edge>::reverse_iterator r_iter;
        for(r_iter = edge.rbegin(); r_iter != edge.rend(); r_iter++) {
            reverPath.edge.push_back(*r_iter);
        }
        return reverPath;
    }

    int len() const {
        return edge.size(); 
    }

    const Edge get(int index) const {
        return edge[index];
    }

private:
    vector<Edge> edge;
};

// 我觉得乘号比较合适
const set<Path> operator * (const set<Path> &lhs, const set<Path> &rhs);


class Dis {
public:
    int value;			//换乘次数
    set<Path> path;		//所有路径
    bool initialized;	//是否被初始化//已经没有使用了

    // methods
    Dis() : value(0), initialized(false) {}
    ~Dis() {
        path.clear();
    }

    void addPath(const Path &t) {
        path.insert(t);
    }

    void addPath(const set<Path> &t) {
        //std::cout << "addPath begin" << " current path size is " << path.size() << ", set<Path> &t's size is " << t.size() << std::endl;
        /*
        std::set_union(std::begin(path), std::end(path),
                       std::begin(t), std::end(t),
                       inserter(path, std::begin(path)));
        */

        for(auto &i : t) {
            path.insert(i);
        }
        //std::cout << "addPath end" << std::endl;
    }

    void assignPath(const set<Path> &other) {
        path.clear();
        path = other;
    }

    void assignReversedPaths(const Dis & other) {
        path.clear();
        for(auto i : other.path) {
            path.insert(i.reversePath());
        }
    }
};


/*
const vector<Path> operator * (const vector<Path> &lhs, const vector<Path> &rhs) {
    vector<Path> res;
    for(int i = 0; i < lhs.size(); i++) {
        for(int j = 0; j < rhs.size(); j++) {
            res.push_back(lhs[i] + rhs[j]); 
        }
    }
    return res;
}
*/
#endif
