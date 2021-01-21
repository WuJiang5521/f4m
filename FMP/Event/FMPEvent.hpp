//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMPEVENT_HPP
#define FMP_FMPEVENT_HPP

#include <iostream>
#include <list>
#include "../Attribute/FMPAttribute.hpp"

class FMPEvent {
public:
    explicit FMPEvent(std::vector<int>);
    friend std::ostream & operator <<(std::ostream &out, const FMPEvent &event);

    int& operator [](int i);
    int operator [](int i) const;
    [[nodiscard]] int size() const;

    bool operator ==(const FMPEvent & e) const;

private:
    std::vector<int> attrs;
};


#endif //FMP_FMPEVENT_HPP
