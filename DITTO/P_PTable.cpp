//
// Created by GuoZiYang on 2021/2/1.
//

#include "P_PTable.h"

std::vector<std::vector<int>> P_PTable::preTable;
std::vector<std::vector<int>> P_PTable::lastTable;
std::vector<std::vector<int>> *P_PTable::table = &preTable;
std::vector<DittoPattern*> P_PTable::patternIDTable;
int P_PTable::tableSize = 1000;
P_PTable::MyHashMap P_PTable::patternIDMap;

void P_PTable::checkTable() {
    table = table == &preTable ? &lastTable : &preTable;
}

void P_PTable::clearTable(int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            (*table)[i][j] = 0;
        }
    }
}

void P_PTable::rollbackTable() {
    table = table == &preTable ? &lastTable : &preTable;
}
