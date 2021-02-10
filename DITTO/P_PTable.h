//
// Created by GuoZiYang on 2021/2/1.
//

#ifndef LSH_P_PTABLE_H
#define LSH_P_PTABLE_H

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
    }; // a hash map for mapping from DittoPattern* to pattern-id
public:
    static std::vector<std::vector<int>> *table; // A table point which points to the current using table.
                                                 // (*table)[i][j] presents the number of sequences that pattern i and pattern j both emerge
    static std::vector<std::vector<int>> preTable; // used for rollback
    static std::vector<std::vector<int>> lastTable; // used for rollback
    static std::vector<DittoPattern*> patternIDTable; // mapping from pattern-id to DittoPattern*
    static void checkTable(); // switch *table to preTable if it points to lastTable, and switch *table to lastTable if it points to preTable
    static void clearTable(int size); // clear *table to all zeros
    static void rollbackTable(); // switch *table back
    static int tableSize; // current size of preTable and lastTable (preTable and lastTable have same size)

    static MyHashMap patternIDMap; // mapping from DittoPattern* to pattern-id
};


#endif //LSH_P_PTABLE_H
