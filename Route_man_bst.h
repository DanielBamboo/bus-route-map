#ifndef __ROUTE_MAN_BST__
#define __ROUTE_MAN_BST__
//这是我的Route_man，但是在改写JAVA->C++的过程中出现了一点点问题
//Route_man 应该是 Route_man<int, Route *> 这样来创建，这样我就可以用nullptr来代替所有的null了
#include <iostream>
#include <vector>


using namespace std;

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

template <class Value = Route *>
class Route_man {
private:

class Node {
public:
	int key;
	Value val;
	Node *left, * right;
	int N;
	bool color;
	
	~Node() {
		
	}
	
public:
	Node(int key, Value val, int N, bool color)
		: key(key), val(val), left(nullptr), right(nullptr), N(N), color(color) {
		cout << "constructing Node" << endl;
	}
};

public:
	Route_man() : root(nullptr) {
		
	}

	static const bool BLACK = false;
	static const bool RED = true;
	
	Node * root;
	
	//------------整合部分(api)--------------------
	
	//这边即插即用的话可能要改指针->引用
	Value &operator [] (int route_index) {
		if(get(route_index) == nullptr) {
			cout << "operator[] : this path doesn't exists" << endl;
			exit(0);
		}
		return getValue(route_index);
	}
	
	bool check_index_legal(int index) {
		return get(index) != nullptr;
	}
	
	void clear() {
		clear(root);
	}
	
	void clear(Node *&x) {
		if(x == nullptr) return;
		cout << "cl" << ' ';
		clear(x->left);
		clear(x->right);
		delete x;
		x = static_cast<Node *>(0);
	}
	
	// 我觉得这个可以改成<int, count>的形式，也就是在记录的时候记下
	// 比如，<二中, 1> --> <二中, 2>等到二中为0的时候就可以不记录了
	// 现在先写一个遍历版本的check_exist_stop()，然后以后再更改。
	bool check_exist_stop(int number) {
		check_exist_stop(root, number);
	}
	
	bool check_exist_stop(Node *h, int number) {
		if(h == nullptr) return false;
        if(check_exist_stop(h->left, number) || check_exist_stop(h->right, number))
			return true;
        for(int i = 0; i < (*(h->val)).stops.size(); i++) {
            if((*(h->val)).stops[i] == number) {
				return true;
			}
		}
		return false;
	}
	
	//因为原本存在的del，所以这边可能要改api? 不改api该bst的方法
	//删除某条公交线路
	void del(int route_index) {
		remove(route_index);
	}
	
	void addRoute(int route_index) {
		put(route_index, new Route(route_index));
	}
	
	int debug_size() {
		return debug_size(root);
	}
	
	int debug_size(Node *x) {
		if(x == nullptr) return 0;
		return (debug_size(x->left) + debug_size(x->right) + 1);
	}
	
	vector<int> keys() {
		if(isEmpty()) return vector<int>();
		return keys(min(), max());
	}
	
	vector<int> keys(int lo, int hi) {
		vector<int> vec;
		keys(root, vec, lo, hi);
		return vec;
	}
	
	void keys(Node * x, vector<int> &vec, int lo, int hi) {
		if(x == nullptr) return;
		if(lo < x->key) 	keys(x->left, vec, lo, hi);
		if(lo <= x->key && hi >= x->key)	vec.push_back(x->key);
		if(hi > x->key) keys(x->right, vec, lo, hi);
	}
	
	// ----------------------------------------
	
	
	
	bool isRed(Node *x) {
		if(x == nullptr) return false;
		return x->color == RED;
	}
	
	bool isEmpty() {
		return root == nullptr;
	}

	Node * rotateLeft(Node *h) {
		cout << "rotateLeft:" << static_cast<char>(h->key) << endl;
		Node *x = h->right;
		h->right = x->left;
		x->left = h;
		x->color = h->color;
		h->color = RED;
		x->N = h->N;
		h->N = 1 + size(h->left) + size(h->right);
		return x;
	}
	
	Node * rotateRight(Node *h) {
		cout << "rotateRight:" << static_cast<char>(h->key) << endl;
		Node *x = h->left;
		h->left = x->right;
		x->right = h;
		x->color = h->color;
		h->color = RED;
		x->N = h->N;
		h->N = 1 + size(h->left) + size(h->right);
		return x;
	}
	
	void flipColors(Node *h) {
		cout << "flip colors" << endl;
		//h->color = RED;
		//h->left->color = BLACK;
		//h->right->color = RED;
		h->color = !h->color;
		h->left->color = !h->left->color;
		h->right->color = !h->right->color;
	}
	
	int size() {
		return size(root);
	}
	
	int size(Node *h) {
		if(h == nullptr) return 0;
		else return h->N;
	}
	
	void put(int key, Value val) {
		root = put(root, key, val);
		cout << "root = " << (int)root->key << endl;
		//prePostTraverse(root);
		cout << "\n\n-------------------\n\n";
		root->color = BLACK;
	}
	
	Node * put(Node *h, int key, Value val) {
		cout << "put(multiple params):" << static_cast<int>(key) << endl;
		if(h == nullptr)
			return new Node(key, val, 1, RED);
		
		if(key < h->key)		h->left = put(h->left, key, val);
		else if(key > h->key)	h->right = put(h->right, key, val);
		else 					h->val = val;
		
		if(isRed(h->right) && !isRed(h->left))		h = rotateLeft(h);
		if(isRed(h->left) && isRed(h->left->left))	h = rotateRight(h);
		if(isRed(h->left) && isRed(h->right))		flipColors(h);
		
		h->N = size(h->left) + size(h->right) + 1;
		return h;
	}
	
	//删除操作部分，这部分代码挺复杂的
	Node * moveRedLeft(Node *h) {
		cout << "moveRedLeft : " << static_cast<int>(h->key) << endl;
		// 假设结点h为红色，h->left 和 h->left->left 都是黑色
		// 将h->left 或者 h->left 的子节点之一变红
		flipColors(h);
		if(isRed(h->right->left)) {
			cout << "moveRedRight isRed(h->left->left)" << endl;

			h->right = rotateRight(h->right);
			h = rotateLeft(h);
			flipColors(h);
		}
		return h;
	}
	
	void deleteMin() {
		if(!isRed(root->left) && !isRed(root->right)) {
			root->color = RED;
		}
		root = deleteMin(root);
		if(!isEmpty()) root.color = BLACK;
	}
	
	Node * deleteMin(Node *h) {
		cout << "deleteMin : " << static_cast<int>(h->key) << endl;

		if(h->left == nullptr)
			return nullptr;
		if(!isRed(h->left) && !isRed(h->left->left))
			h = moveRedLeft(h);
		h->left = deleteMin(h->left);
		return balance(h);
	}
	
	Node * balance(Node *h) {
		cout << "balance : " << static_cast<int>(h->key) << endl;

		if(isRed(h->right))		h = rotateLeft(h);
		if(isRed(h->left) && isRed(h->left->left))	h = rotateRight(h);
		if(isRed(h->left) && isRed(h->right))		flipColors(h);
		
		h->N = size(h->left) + size(h->right) + 1;
		return h;
	}
	
	Node * moveRedRight(Node *h) {
		cout << "moveRedLeft : " << static_cast<int>(h->key) << endl;

		flipColors(h);
		if(isRed(h->left->left)) {
			cout << "moveRedRight isRed(h->left->left)" << endl;
			h = rotateRight(h);
			flipColors(h);
		}
		return h;
	}
	
	void deleteMax() {
		if(!isRed(root->left) && !isRed(root->right)) {
			root->color = RED;
		}
		root = deleteMax(root);
		if(!isEmpty()) root->color = BLACK;
	}
	
	Node * deleteMax(Node * h) {
		cout << "deleteMax : " << static_cast<int>(h->key) << endl;

		if(isRed(h->left)) h = rotateRight(h);
		if(h->right == nullptr)	return nullptr;
		if(!isRed(h->right) && !isRed(h->right->left))
			h = moveRedRight(h);
		h->right = deleteMax(h->right);
		return balance(h);
	}
	
	//为了给api让路，把所有的del换成remove
	void remove(int key) {
		if(!isRed(root->left) && !isRed(root->right))
			root->color = RED;
		root = remove(root, key);
		if(!isEmpty()) root->color = BLACK;
	}
	
	Node * remove(Node * h, int key) {
		cout << "remove : " << static_cast<int>(h->key) << " for :" << static_cast<int>(key) << endl;

		if(key < h->key) {
			if(!isRed(h->left) && !isRed(h->left->left))
				h = moveRedLeft(h);
			h->left = remove(h->left, key);
		}
		else {
			if(isRed(h->left))
				h = rotateRight(h);
			if(key == h->key && h->right == nullptr)
				return nullptr;
			if(!isRed(h->right) && !isRed(h->right->left))
				h = moveRedRight(h);
			if(key == h->key) {
				cout << "hit target" << endl;
				Node *x = min(h->right);
				h->key = x->key;
				h->val = x->val;
				/*
				h->val = get(h->right, min(h->right)->key);
				h->key = min(h->right)->key;
				*/
				h->right = deleteMin(h->right);
			}
			else h->right = remove(h->right, key);
		}
		return balance(h);
	}
	
	//我不知道在C++里面null对应的是什么，Route是一个类，而JAVA里面的类都是以引用的形式出现的，所以可以用null表示空，但是C++里面好像没有这样的表示方法
	Value get(int key) { 
		//如果要改成Route_man，那么这里是需要改动的
		/*
        if (!isLegal(key)) {
			exit(0);
		}
		*/
		
        return get(root, key);
    }

    // value associated with the given key in subtree rooted at x; null if no such key
    Value get(Node * x, int key) {
        while (x != nullptr) {
            if(key < x->key) x = x->left;
            else if(key > x->key) x = x->right;
            else return x->val;
        }
        return nullptr;
    }
	
	Value &getValue(int key) {
		if(!check_index_legal(key)) {
			cout << "this route doesn't exitst" << endl;
			exit(0);
		};
		return getValue(root, key);
	}
	
	Value &getValue(Node * x, int key) {
		while(x != nullptr) {
			if(key < x->key) x = x->left;
            else if(key > x->key) x = x->right;
            else return x->val;
		}
	}
	
	int min() {
        if (isEmpty()) exit(0);
        return min(root)->key;
    }

    // the smallest key in subtree rooted at x; null if no such key
    Node * min(Node * x) { 
        // assert x != null;
        if (x->left == nullptr) return x; 
        else return min(x->left); 
    } 
	
	int max() {
		if(isEmpty()) exit(0);
		return max(root)->key;
	}
	
	Node * max(Node * x) {
		if(x->right == nullptr) return x;
		else					return max(x->right);
	}
	
	
	void prePostTraverse(Node *x) {
		if(x == nullptr) {
			//cout << "x == nullptr return" << endl;
			return;
		}
		//cout << (x->color == RED?"RED=":"BLACK-") << static_cast<char>(x->key) << " ";
		prePostTraverse(x->left);
		prePostTraverse(x->right);
	}
	
	void inPostTraverse(Node *x) {
		if(x == nullptr) {
			//cout << "x == nullptr return" << endl;
			return;
		}
		inPostTraverse(x->left);
		//cout << (x->color == RED?"RED=":"BLACK-") << static_cast<char>(x->key) << " ";
		inPostTraverse(x->right);
	}
};


/*
int main() {
	
	cout << "normal start" << endl;
/*
	Route_man<int *> route_man;	
	char a[11] = "SEARCHXMPL";
	for(int i = 0; i < 10; i++) {
		route_man.put(a[i], new int(i));
	}
	route_man.prePostTraverse(route_man.root);
	cout << endl;
	route_man.inPostTraverse(route_man.root);
	cout << endl;
	
	route_man.remove('X');
	
	route_man.prePostTraverse(route_man.root);
	cout << endl;
	route_man.inPostTraverse(route_man.root);
	
	cout << endl;
	cout << *route_man['A'];
	cout << endl;
	cout << route_man.size() << endl;
	route_man.clear(); cout << endl;
	cout << route_man.size() << endl;
	cout << static_cast<void *>(route_man.root) << endl;
	cout << static_cast<void *>(nullptr) << endl;
////
	Route_man<Route *> man;
    man.addRoute(2);
    std::cout << man.debug_size() << std::endl;
    for(int i = 0; i < 5; i++)
        man[2]->stops.push_back(i);

    std::cout << man.debug_size() << std::endl;

    man.addRoute(5);
    for(int i = 5; i < 10; i++)
        man[5]->stops.push_back(i);

    man.addRoute(3);
    for(int i = 10; i < 15; i++)
        man[3]->stops.push_back(i);

    man.addRoute(1);
    for(int i = 10; i < 15; i++)
        man[1]->stops.push_back(i);

    std::cout << "add done!" << std::endl;
    std::cout << man.debug_size() << std::endl;
	
	//如何遍历这个bst
	man.get(1)->stops.push_back(666);
	
	vector<int> keys = man.keys();
	cout << "traverse this tree" << endl;
	for(auto i : keys) {
		vector<int> &stops = man.get(i)->stops;
		cout << i << endl;
		for(int j = 0; j < stops.size(); j++) {
			cout << (stops)[j] << " ";
		}
		cout << endl;
	}

	cout << "normal complete" << endl;
	return 0;
}
*/

#endif
