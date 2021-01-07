//
// Created by A on 2020/12/22.
//

#ifndef FMP_SEQUENCE_HPP
#define FMP_SEQUENCE_HPP

#include <iostream>
#include <list>
#include <vector>
#include <set>
#include "../Event/Event.hpp"
#include "../Pattern/Pattern.hpp"
#include "../Types.h"

class Sequence {
private:
    typedef std::pair<std::pair<int, int>, std::vector<int> > OccurType;
public:
    explicit Sequence(std::vector<Event> );
    friend std::ostream & operator <<(std::ostream &out, const Sequence &sequence);

    void getCover(CodeTableType &PS);

    static const int coverMissFlag;

    struct occurSetComp {
        bool operator() (const OccurType &a, const OccurType & b);
    };

    int size() const;

    std::vector<std::vector<std::pair<const Pattern *, int>>> cover;

private:
    std::vector<Event> events;

    typedef std::set<std::pair<std::pair<int, int>, std::vector<int> >, occurSetComp> OccursType;

    void getOccur(OccursType &, const Pattern &) const;
    bool hasInterSection(const OccurType &, const Pattern &) const;
    bool coverIsFull() const;
};


#endif //FMP_SEQUENCE_HPP
