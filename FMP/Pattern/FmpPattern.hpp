//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMPPATTERN_HPP
#define FMP_FMPPATTERN_HPP


#include <list>
#include <set>
#include "../Event/FMPEvent.hpp"

class FmpPattern {
public:
    explicit FmpPattern(std::vector<FMPEvent>  );
    FMPEvent& operator[] (int i);
    [[nodiscard]] int size() const;
    std::vector<FMPEvent>::iterator begin();
    std::vector<FMPEvent>::iterator end();
    void push_back(const FMPEvent &);
    const FMPEvent& operator[] (int i) const;
    static const int patternNULLFlag;
    struct codeTableSetComp {
        bool operator ()(const FmpPattern* const & a, const FmpPattern* const &b) const;
    };
    void clearCnts();
    [[nodiscard]] int getUsage() const;
    [[nodiscard]] int getGaps() const;
    [[nodiscard]] int getFills() const;
    void rollback();
    void checkup();
    static int totalTimeStamp;
    int timeStamp;
    friend class FMPSequence;
    friend class FMP;
private:
    std::vector<FMPEvent> pattern;
    int usg, gaps, fills;
    int oldUsg, oldGaps, oldFills;
    bool disabled;
};




#endif //FMP_FMPPATTERN_HPP
