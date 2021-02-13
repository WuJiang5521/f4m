//
// Created by GuoZiYang on 2021/2/1.
//

#include "P_P_Table.h"

std::vector<std::vector<int>> P_P_Table::pre_table;
std::vector<std::vector<int>> P_P_Table::last_table;
std::vector<std::vector<int>> *P_P_Table::table = &pre_table;
std::vector<Pattern*> P_P_Table::pattern_id_table;
int P_P_Table::table_size = 1000;
P_P_Table::My_hash_map P_P_Table::pattern_id_map;

void P_P_Table::check_table() {
    table = table == &pre_table ? &last_table : &pre_table;
}

void P_P_Table::clear_table(int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            (*table)[i][j] = 0;
        }
    }
}

void P_P_Table::rollback_table() {
    table = table == &pre_table ? &last_table : &pre_table;
}
