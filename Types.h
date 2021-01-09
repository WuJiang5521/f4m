//
// Created by GuoZiYang on 2021/1/7.
//

#ifndef FMP_TYPES_H
#define FMP_TYPES_H

#include "Pattern/Pattern.hpp"

typedef std::set<Pattern*, Pattern::codeTableSetComp> CodeTableType;
typedef std::map<Pattern*, std::map<Pattern*, int>> PatternPatternTableType;
typedef std::map<Pattern*, std::map<Pattern*, bool>> PatternPatternTableValidType;

#endif //FMP_TYPES_H
