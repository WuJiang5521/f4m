//
// Created by A on 2020/12/22.
//

#include "Sequence.hpp"

using namespace std;

Sequence::Sequence(list<Event> events) :
        events(move(events)) {}

ostream &operator<<(ostream &out, const Sequence &sequence) {
    for (auto i = sequence.events.begin(); i != sequence.events.end(); i++)
        out << (i == sequence.events.begin() ? "" : ", ") << *i;
    return out;
}
