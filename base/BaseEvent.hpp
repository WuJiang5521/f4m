//
// Created by A on 2020/12/22.
//

#ifndef FMP_BASEEVENT_HPP
#define FMP_BASEEVENT_HPP

#include <iostream>
#include <list>
#include "BaseAttribute.hpp"

class BaseEvent {
public:
    explicit BaseEvent(std::vector<int>);
    friend std::ostream & operator <<(std::ostream &out, const BaseEvent &event);

    int& operator [](int i);
    int operator [](int i) const;
    [[nodiscard]] int size() const;

    bool operator ==(const BaseEvent & e) const;

private:
    std::vector<int> attrs;
};


#endif //FMP_BASEEVENT_HPP
