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
    [[nodiscard]] int size() const;
    std::vector<Event>::iterator begin();
    std::vector<Event>::iterator end();
    const Event& operator[] (int i) const;
    static const int patternNULLFlag;
    struct codeTableSetComp {
        bool operator ()(const Pattern* const & a, const Pattern* const &b) const;
    };
    void clearCnts();
    [[nodiscard]] int getUsage() const;
    [[nodiscard]] int getGaps() const;
    [[nodiscard]] int getFills() const;
    static int totalTimeStamp;
    int timeStamp;
    friend class Sequence;
private:
    std::vector<Event> pattern;
    int usg, gaps, fills;
};




#endif //FMP_PATTERN_HPP
