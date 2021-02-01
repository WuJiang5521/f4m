//
// Created by GuoZiYang on 2021/2/1.
//

#include "P_PTable.h"

std::map<DittoPattern*, std::map<DittoPattern*, int>> P_PTable::table;
std::map<DittoPattern*, std::map<DittoPattern*, int>> P_PTable::lastTable;
std::map<DittoPattern*, std::map<DittoPattern*, bool>> P_PTable::generatedTable;

void P_PTable::reserveForPattern(DittoPattern * X) {
    table.emplace(make_pair(X, std::map<DittoPattern *, int>()));
    for (auto & it : table) {
        it.second[X] = 0;
        table[X][it.first] = 0;
    }
}

void P_PTable::eraseForPattern(DittoPattern * X) {
    for (auto & it : table) {
        if (it.second.find(X) != it.second.end()) {
            it.second.erase(it.second.find(X));
        }
    }
    table.erase(table.find(X));
}

void P_PTable::checkTable() {
    lastTable = table;
}

void P_PTable::clearTable() {
    for (auto & i : table) {
        for (auto & j : table[i.first]) {
            table[i.first][j.first] = 0;
        }
    }
}

void P_PTable::rollbackTable() {
    table = lastTable;
}
