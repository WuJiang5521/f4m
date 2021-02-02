//
// Created by GuoZiYang on 2021/2/1.
//

#ifndef FMP_P_PTABLE_H
#define FMP_P_PTABLE_H

#include <map>
#include "DittoPattern.h"

class P_PTable {
public:
    static std::vector<std::vector<int>> *table;
    static std::vector<std::vector<int>> preTable;
    static std::vector<std::vector<int>> lastTable;
    static std::vector<DittoPattern*> patternIDTable;
    static std::vector<int> prunedTable;
    static void checkTable();
    static void clearTable(int size);
    static void rollbackTable();
    static int generatedTable;
    static int tableSize;

    static int total_p_id;
};


#endif //FMP_P_PTABLE_H
