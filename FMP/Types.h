//
// Created by GuoZiYang on 2021/1/7.
//

#ifndef FMP_TYPES_H
#define FMP_TYPES_H

#include "Pattern/FmpPattern.hpp"

typedef std::set<FmpPattern*, FmpPattern::codeTableSetComp> CodeTableType;
typedef std::map<FmpPattern*, std::map<FmpPattern*, int>> PatternPatternTableType;
typedef std::map<FmpPattern*, std::map<FmpPattern*, bool>> PatternPatternTableValidType;

#endif //FMP_TYPES_H
