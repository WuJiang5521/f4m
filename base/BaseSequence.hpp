//
// Created by A on 2020/12/22.
//

#ifndef FMP_BASESEQUENCE_HPP
#define FMP_BASESEQUENCE_HPP

#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <numeric>
#include "BaseEvent.hpp"
#include "../FMP/Pattern/FmpPattern.hpp"
#include "../FMP/Types.h"
#include "../FMP/P_PTable/P_PTable.h"

class BaseSequence {
private:
    typedef std::pair<std::pair<int, int>, std::vector<int> > OccurType;
public:
    explicit BaseSequence(std::vector<BaseEvent> );
    std::vector<BaseEvent>::iterator begin();
    std::vector<BaseEvent>::iterator end();
    friend std::ostream & operator <<(std::ostream &out, const BaseSequence &sequence);
//    BaseSequence & operator = (const BaseSequence &);

    void getCover(CodeTableType &PS);

    static const int coverMissFlag;

    struct occurSetComp {
        bool operator() (const OccurType &a, const OccurType & b);
    };

    [[nodiscard]] int size() const;
    std::vector<std::vector<std::pair<const FmpPattern *, int>>> cover;


private:
    std::vector<BaseEvent> events;
    std::map<FmpPattern *, std::vector<int>> coverPattern;

    typedef std::set<std::pair<std::pair<int, int>, std::vector<int> >, occurSetComp> OccursType;

    void getOccur(OccursType &, const FmpPattern &) const;
    bool hasInterSection(const OccurType &, const FmpPattern &) const;
    bool coverIsFull() const;
};


#endif //FMP_BASESEQUENCE_HPP
