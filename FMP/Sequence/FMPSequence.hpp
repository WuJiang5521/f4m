//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMPSEQUENCE_HPP
#define FMP_FMPSEQUENCE_HPP

#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <numeric>
#include "../Event/FMPEvent.hpp"
#include "../Pattern/FmpPattern.hpp"
#include "../Types.h"
#include "../P_PTable/P_PTable.h"

class FMPSequence {
private:
    typedef std::pair<std::pair<int, int>, std::vector<int> > OccurType;
public:
    explicit FMPSequence(std::vector<FMPEvent> );
    std::vector<FMPEvent>::iterator begin();
    std::vector<FMPEvent>::iterator end();
    friend std::ostream & operator <<(std::ostream &out, const FMPSequence &sequence);
//    FMPSequence & operator = (const FMPSequence &);

    void getCover(CodeTableType &PS);

    static const int coverMissFlag;

    struct occurSetComp {
        bool operator() (const OccurType &a, const OccurType & b);
    };

    [[nodiscard]] int size() const;
    std::vector<std::vector<std::pair<const FmpPattern *, int>>> cover;


private:
    std::vector<FMPEvent> events;
    std::map<FmpPattern *, std::vector<int>> coverPattern;

    typedef std::set<std::pair<std::pair<int, int>, std::vector<int> >, occurSetComp> OccursType;

    void getOccur(OccursType &, const FmpPattern &) const;
    bool hasInterSection(const OccurType &, const FmpPattern &) const;
    bool coverIsFull() const;
};


#endif //FMP_FMPSEQUENCE_HPP
