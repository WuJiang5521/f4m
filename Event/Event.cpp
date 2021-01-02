//
// Created by A on 2020/12/22.
//

#include "Event.hpp"
#include "../Pattern/Pattern.hpp"

using namespace std;

Event::Event(vector<int> attrs) :
        attrs(move(attrs)) {}

int& Event::operator[](int i) {
    return attrs[i];
}

int Event::operator[](int i) const {
    return attrs[i];
}

bool Event::operator==(const Event &e) const {
    for (int i = 0; i < attrs.size(); ++i) {
        if (attrs[i] == Pattern::patternNULLFlag || e.attrs[i] == Pattern::patternNULLFlag) {
            continue;
        }
        if (attrs[i] != e.attrs[i]) {
            return false;
        }
    }
    return true;
}

int Event::size() const {
    return attrs.size();
}

ostream &operator<<(ostream &out, const Event &event) {
//    out << "("
//        << Attribute::get_key_value("ttBallPosition", event.attrs.at(0)) << ", "
//        << Attribute::get_key_value("ttStrikePosition", event.attrs.at(1)) << ", "
//        << Attribute::get_key_value("ttStrikeTech", event.attrs.at(2)) << ", "
//        << Attribute::get_key_value("ttSpinKind", event.attrs.at(3)) << ")";

    out << "("
        << event.attrs.at(0) << ", "
        << event.attrs.at(1) << ", "
        << event.attrs.at(2) << ", "
        << event.attrs.at(3) << ")";
    return out;
}
