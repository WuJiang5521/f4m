//
// Created by GuoZiYang on 2021/1/8.
//

#ifndef FMP_P_PTABLE_H
#define FMP_P_PTABLE_H

#include "../Types.h"

class P_PTable {
public:
    static PatternPatternTableType table;
    static PatternPatternTableType lastTable;
    static void reserveForPattern(Pattern *);
    static void eraseForPattern(Pattern *);
    static void checkTable();
    static void clearTable();
    static void rollbackTable();
};


#endif //FMP_P_PTABLE_H
