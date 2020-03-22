#include "setPath.h"

const std::set<Path> operator * (const std::set<Path> &lhs, const std::set<Path> &rhs) {
    std::set<Path> res;
    for(auto i : lhs) {
        for(auto j : rhs) {
            res.insert(i+j);
        }
    }
    return res;
}

