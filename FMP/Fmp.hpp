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

#include "Pattern/FmpPattern.hpp"
#include "Types.h"
#include "MathUtil.h"
#include "P_PTable/P_PTable.h"
#include "Sequence/FMPSequence.hpp"

class FMP {
public:
    static std::list<FMPSequence> sequenceList;
    static int max_seq_len;
    static CodeTableType codeTable;
    static int nST;
    static int D_total_len;
    static std::vector<std::vector<int>> ST_support;

    static void do_fmp();
    static double LCalc(CodeTableType &);

private:
    static std::vector<FmpPattern *> getCand(FmpPattern * X);
    static FmpPattern * nullPattern;
    static void coverSequenceList(std::list<FMPSequence> &, CodeTableType &);
    static void checkCodeTable(CodeTableType&);
    static void clearCodeTable(CodeTableType&);
    static void rollbackCodeTable(CodeTableType&);

    static FmpPattern * getAlignment(FmpPattern *, FmpPattern *, int);

    static void prune(std::list<FMPSequence> &, CodeTableType &, double &);
};


#endif //FMP_FMP_HPP
