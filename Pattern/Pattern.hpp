//
// Created by A on 2020/12/22.
//

#ifndef FMP_PATTERN_HPP
#define FMP_PATTERN_HPP


#include <list>
#include <set>
#include "../Event/Event.hpp"

class Pattern {
public:
    explicit Pattern(std::vector<Event>  );
    Event& operator[] (int i);
    int size() const;
    const Event& operator[] (int i) const;
    static const int patternNULLFlag;
    struct codeTableSetComp {
        bool operator ()(const Pattern & a, const Pattern &b) const;
    };
    static int totalTimeStamp;
    int timeStamp;
private:
    std::vector<Event> pattern;
};




#endif //FMP_PATTERN_HPP
