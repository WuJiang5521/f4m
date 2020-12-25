//
// Created by A on 2020/12/22.
//

#ifndef FMP_EVENT_HPP
#define FMP_EVENT_HPP


#include <list>
#include "Attribute.hpp"

class Event {
public:
    explicit Event(std::vector<int>);
    friend std::ostream & operator <<(std::ostream &out, const Event &event);

private:
    std::vector<int> attrs;
};


#endif //FMP_EVENT_HPP
