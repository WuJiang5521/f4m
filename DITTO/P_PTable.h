//
// Created by GuoZiYang on 2021/2/1.
//

#ifndef FMP_P_PTABLE_H
#define FMP_P_PTABLE_H

#include <map>
#include "DittoPattern.h"

class P_PTable {
    class MyHashMap {
        const int MOD = 1000007;
        int a[1000007]{};
    public:
        MyHashMap() {memset(a, 0, sizeof a);}
        int operator[](DittoPattern* x) const {
            return a[(unsigned long long)x % MOD];
        }
        int& operator[](DittoPattern* x) {
            return a[(unsigned long long)x % MOD];
        }
        void clear() {
            memset(a, 0, sizeof a);
        }
    };
public:
    static std::vector<std::vector<int>> *table;
    static std::vector<std::vector<int>> preTable;
    static std::vector<std::vector<int>> lastTable;
    static std::vector<DittoPattern*> patternIDTable;
    static void checkTable();
    static void clearTable(int size);
    static void rollbackTable();
    static int tableSize;

    static int total_p_id;
    static MyHashMap patternIDMap;
};


#endif //FMP_P_PTABLE_H
