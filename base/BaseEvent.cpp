//
// Created by A on 2020/12/22.
//

#include "BaseEvent.hpp"
#include "../FMP/Pattern/FmpPattern.hpp"

using namespace std;

BaseEvent::BaseEvent(vector<int> attrs) :
        attrs(move(attrs)) {}

int& BaseEvent::operator[](int i) {
    return attrs[i];
}

int BaseEvent::operator[](int i) const {
    return attrs[i];
}

bool BaseEvent::operator==(const BaseEvent &e) const {
    for (int i = 0; i < attrs.size(); ++i) {
        if (attrs[i] == FmpPattern::patternNULLFlag || e.attrs[i] == FmpPattern::patternNULLFlag) {
            continue;
        }
        if (attrs[i] != e.attrs[i]) {
            return false;
        }
    }
    return true;
}

int BaseEvent::size() const {
    return attrs.size();
}

ostream &operator<<(ostream &out, const BaseEvent &event) {
//    out << "("
//        << BaseAttribute::get_key_value("ttBallPosition", event.attrs.at(0)) << ", "
//        << BaseAttribute::get_key_value("ttStrikePosition", event.attrs.at(1)) << ", "
//        << BaseAttribute::get_key_value("ttStrikeTech", event.attrs.at(2)) << ", "
//        << BaseAttribute::get_key_value("ttSpinKind", event.attrs.at(3)) << ")";

    out << "("
        << event.attrs.at(0) << ", "
        << event.attrs.at(1) << ", "
        << event.attrs.at(2) << ", "
        << event.attrs.at(3) << ")";
    return out;
}
