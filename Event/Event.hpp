//
// Created by A on 2020/12/22.
//

#ifndef FMP_EVENT_HPP
#define FMP_EVENT_HPP

#include <iostream>
#include <list>
#include "../Attribute/Attribute.hpp"

class Event {
public:
    explicit Event(std::vector<int>);
    friend std::ostream & operator <<(std::ostream &out, const Event &event);

    int& operator [](int i);
    int operator [](int i) const;
    [[nodiscard]] int size() const;

    bool operator ==(const Event & e) const;

private:
    std::vector<int> attrs;
};


#endif //FMP_EVENT_HPP
