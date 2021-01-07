//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMP_HPP
#define FMP_FMP_HPP

#include <list>
#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <cmath>
#include <algorithm>

#include "Sequence/Sequence.hpp"
#include "Pattern/Pattern.hpp"
#include "Types.h"
#include "MathUtil.h"

class FMP {
public:
    static std::list<Sequence> sequenceList;
    static int max_seq_len;
    static CodeTableType codeTable;
    static int nST;
    static int D_total_len;
    static std::vector<std::vector<int>> ST_support;
    static void do_fmp();
    static double LCalc();
};


#endif //FMP_FMP_HPP
