//
// Created by GuoZiYang on 2021/1/8.
//

#include "P_PTable.h"

PatternPatternTableType P_PTable::table;
PatternPatternTableType P_PTable::lastTable;

void P_PTable::reserveForPattern(Pattern * X) {
    table.emplace(make_pair(X, std::map<Pattern *, int>()));
    for (auto & it : table) {
        it.second[X] = 0;
        table[X][it.first] = 0;
    }
}

void P_PTable::eraseForPattern(Pattern * X) {
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