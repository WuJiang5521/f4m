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

class Sequence {
public:
    explicit Sequence(std::vector<Event> );
    friend std::ostream & operator <<(std::ostream &out, const Sequence &sequence);

    void getCover(std::set<Pattern, Pattern::codeTableSetComp> &PS);

    static const int coverMissFlag;

    struct occurSetComp {
        bool operator() (const std::pair<int, std::vector<int> > &a, const std::pair<int, std::vector<int> > & b);
    };

    int size() const;

    std::vector<std::vector<std::pair<const Pattern *, int>>> cover;

private:
    std::vector<Event> events;

    void getOccur(std::set<std::pair<int, std::vector<int> >, occurSetComp> &, const Pattern &);
    bool hasInterSection(const std::pair<int, std::vector<int> > &, const Pattern &) const;
    bool coverIsFull() const;
};


#endif //FMP_SEQUENCE_HPP
