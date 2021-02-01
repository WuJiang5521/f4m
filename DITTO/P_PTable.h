//
// Created by GuoZiYang on 2021/2/1.
//

#ifndef FMP_P_PTABLE_H
#define FMP_P_PTABLE_H

#include <map>
#include "DittoPattern.h"

class P_PTable {
public:
    static std::map<DittoPattern*, std::map<DittoPattern*, int>> table;
    static std::map<DittoPattern*, std::map<DittoPattern*, int>> lastTable;
    static void reserveForPattern(DittoPattern *);
    static void eraseForPattern(DittoPattern *);
    static void checkTable();
    static void clearTable();
    static void rollbackTable();
    static std::map<DittoPattern*, std::map<DittoPattern*, bool>> generatedTable;

};


#endif //FMP_P_PTABLE_H
