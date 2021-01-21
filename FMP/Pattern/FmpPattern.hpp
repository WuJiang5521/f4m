//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMPPATTERN_HPP
#define FMP_FMPPATTERN_HPP


#include <list>
#include <set>
#include "../../base/BaseEvent.hpp"

class FmpPattern {
public:
    explicit FmpPattern(std::vector<BaseEvent>  );
    BaseEvent& operator[] (int i);
    [[nodiscard]] int size() const;
    std::vector<BaseEvent>::iterator begin();
    std::vector<BaseEvent>::iterator end();
    void push_back(const BaseEvent &);
    const BaseEvent& operator[] (int i) const;
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
    friend class BaseSequence;
    friend class FMP;
private:
    std::vector<BaseEvent> pattern;
    int usg, gaps, fills;
    int oldUsg, oldGaps, oldFills;
    bool disabled;
};




#endif //FMP_FMPPATTERN_HPP
