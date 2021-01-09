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
#include "P_PTable/P_PTable.h"

class FMP {
public:
    static std::list<Sequence> sequenceList;
    static int max_seq_len;
    static CodeTableType codeTable;
    static int nST;
    static int D_total_len;
    static std::vector<std::vector<int>> ST_support;

    static void do_fmp();
    static double LCalc(CodeTableType &);

private:
    static std::vector<Pattern *> getCand(Pattern * X);
    static Pattern * nullPattern;
    static void coverSequenceList(std::list<Sequence> &, CodeTableType &);
    static void checkCodeTable(CodeTableType&);
    static void clearCodeTable(CodeTableType&);
    static void rollbackCodeTable(CodeTableType&);

    static Pattern * getAlignment(Pattern *, Pattern *, int);

    static void prune(std::list<Sequence> &, CodeTableType &, double &);
};


#endif //FMP_FMP_HPP
