#ifndef __ROUTE_MAN_H__
#define __ROUTE_MAN_H__

#include <vector>
#include <unordered_set>
#include <iostream>
#include <iterator>
#include <fstream>
#include <QDebug>
#include <QVector>
#include <QPointF>
#include <string>

/*
似乎只能用在保存线路不能用来保存查询结果
这是一个用于保存路径的类
Route结构体用来保存路径的信息
*/

struct Route {
    std::vector<int> stops;
    int num; //线路的编号
    struct Route *next;
    struct Route *pre;

    Route() :next(nullptr), pre(nullptr) {}
    Route(int num) :num(num) {
        stops.reserve(10);
    }
    void addStop(int index) {
        stops.push_back(index);
    }
    
    //void addStop(...)
    // 删除第index位置的元素，从1数起
    void delStop(int index) {
        std::vector<int>::iterator it = stops.begin();
        stops.erase(it + index - 1);
    }

    //增加编号为num的站点在stops数组的index之前，这个number由map分配，map[cnt++] = string
    void insertStop(int number, int index) {
        stops.insert(stops.begin() + index - 1, number);
    } 
};

template<typename T>
class Route_man_Iterator;

//TODO
//把Route_man改成类模板

// set::erase() 删除特定元素
// route management
template<typename T = Route>
class Route_man {
    friend class Route_man_Iterator<T>;
private:
    int num;		//公交线路数量
    Route *head;
    Route *tail;
    std::unordered_set<int> route_indexes;

public:
    Route_man(): num(0), head(nullptr), tail(nullptr) {

    }

    ~Route_man() {
        Route *pi = head, *pre = nullptr;
        while(pi) {
            pre = pi;
            pi = pi->next;
            delete pre;
        }
    }

    bool check_exist_stop(int number) {
        Route *pi = head;
        while(pi) {
            for(int i = 0; i < pi->stops.size(); i++) {
                if(pi->stops[i] == number) {
                    return true;
                }
            }
            pi = pi->next;
        }
        return false;
    }

    void clear() {
        num = 0;
        route_indexes.clear();
        Route *pi = head, *pre = nullptr;
        while(pi) {
            pre = pi;
            pi = pi->next;
            delete pre;
        }
        head = tail = nullptr;
    }
    // 输入线路，返回引用
    Route &operator [] (int route_index) {
        if(!check_index_legal(route_index)) {
            std::cout << "没有这条路" << std::endl;
            exit(0);
        }
        Route *pi = head;
        while(pi) {
            if(pi->num == route_index) {
                break;
            }
            pi = pi->next;
        }
        return *pi;
    }

    size_t size() {
        return num;
    }

    bool check_index_legal(int index) {
        if(route_indexes.count(index) == 0)
            return false;
        return true;
    }

    // 删除某条公交线路
    void del(int route_index) {
        if(!check_index_legal(route_index)) {
            std::cout << "你在尝试删除一条不存在的道路";
            exit(0);
        }
        Route *pi = head;
        while(pi) {
            if(pi->num == route_index) {
                std::cout << "we got that" << std::endl;
                break;
            }
            pi = pi->next;
        }
        if(pi == head) {
            head = head->next;
            head->pre = nullptr;

            delete pi;
        } else if(pi == tail) {
            tail->pre->next = nullptr;
            tail = pi->pre;
            delete pi;
        }else {
            pi->pre->next = pi->next;
            pi->next->pre = pi->pre;
            
            delete pi;
        }
        num--;
        route_indexes.erase(route_index);
    }

    void addRoute(int route_index) {
        route_indexes.insert(route_index);
        Route *newR = new Route(route_index);
        if(tail == nullptr) {
            std::cout << "tail == nullptr" << std::endl;
            if(!newR) exit(0);
            head = newR;
            tail = newR;
        } else {
            tail->next = newR;
            tail->next->pre = tail;
            tail = tail->next;
        }
        tail->next = nullptr;
        num++;
    }

    int debug_size() {
        Route *pi = head;
        int cnt = 0;
        while(pi) {
            cnt++;
            pi = pi->next;
        }
        return cnt;
    }

    /*
    void writeToFile(const char * filename, std::vector<std::string> names, QVector<QPointF> poses) {
        using namespace std;
        fstream fout(filename, ios_base::out | ios_base::trunc);
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

        fout << names.size() << endl;
        for(int i = 0; i < names.size(); i++) {
            fout << names[i] << "\t\t" << (int)poses[i].x() << " " << (int)poses[i].y() << endl;
        }


        fout << num;

        Route *pi = head;
        while(pi) {
            fout << pi->num << endl;
            fout << pi->stops.size() << endl;
            for(auto i : pi->stops) {
                fout <<
            }
        }
    }
    */

    Route_man_Iterator<T> begin() {
        return Route_man_Iterator<T>(*this);
    }

    Route_man_Iterator<T> end() {
        Route_man_Iterator<T> end_iter(*this);
        end_iter.value = nullptr;
        /*
        while((end_iter.value)->next != nullptr) {
            end_iter.value = (end_iter.value)->next;
        }
        */
        return end_iter;
    }
};

// std::vector::iterator it = vec.begin() + 10
// vec.erase(it)
//


template<typename T = Route>
class Route_man_Iterator : public std::iterator<std::forward_iterator_tag, T> {
    friend class Route_man<T>;
protected:
    Route_man<T> &route_man;
    T *value;
public:
    explicit Route_man_Iterator(Route_man<T> & a_route_man) : route_man(a_route_man), value(a_route_man.head) {}

    Route_man_Iterator<T> & operator = (const Route_man_Iterator<T> & src) {
        route_man = src.route_man;
        value = src.value;
    }

    T & operator * () {
        return *value;
    }

    //Prefix increment operator
    Route_man_Iterator<T> & operator ++() {
        value = value->next;
        return *this;
    }

    //Postfix increment operator
    Route_man_Iterator<T> operator ++(int) {
        auto temp = *this;
        value = value->next;
        return temp;
    }

    bool operator != (const Route_man_Iterator & rhs) { return value != rhs.value; }
};




#endif
