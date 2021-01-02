//
// Created by A on 2020/12/22.
//

#ifndef FMP_SEQUENCE_HPP
#define FMP_SEQUENCE_HPP

#include <iostream>
#include <list>
#include "Event/Event.hpp"

class Sequence {
public:
    explicit Sequence(std::list<Event> );
    friend std::ostream & operator <<(std::ostream &out, const Sequence &sequence);
private:
    std::list<Event> events;
};


#endif //FMP_SEQUENCE_HPP
