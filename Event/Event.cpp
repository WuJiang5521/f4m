//
// Created by A on 2020/12/22.
//

#include "Event.hpp"

using namespace std;

Event::Event(vector<int> attrs) :
        attrs(move(attrs)) {}

ostream &operator<<(ostream &out, const Event &event) {
    out << "("
        << Attribute::get_key_value("ttBallPosition", event.attrs.at(0)) << ", "
        << Attribute::get_key_value("ttStrikePosition", event.attrs.at(1)) << ", "
        << Attribute::get_key_value("ttStrikeTech", event.attrs.at(2)) << ", "
        << Attribute::get_key_value("ttSpinKind", event.attrs.at(3)) << ")";
    return out;
}
