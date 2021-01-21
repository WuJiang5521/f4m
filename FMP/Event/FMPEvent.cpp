//
// Created by A on 2020/12/22.
//

#include "FMPEvent.hpp"
#include "../Pattern/FmpPattern.hpp"

using namespace std;

FMPEvent::FMPEvent(vector<int> attrs) :
        attrs(move(attrs)) {}

int& FMPEvent::operator[](int i) {
    return attrs[i];
}

int FMPEvent::operator[](int i) const {
    return attrs[i];
}

bool FMPEvent::operator==(const FMPEvent &e) const {
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

int FMPEvent::size() const {
    return attrs.size();
}

ostream &operator<<(ostream &out, const FMPEvent &event) {
//    out << "("
//        << FMPAttribute::get_key_value("ttBallPosition", event.attrs.at(0)) << ", "
//        << FMPAttribute::get_key_value("ttStrikePosition", event.attrs.at(1)) << ", "
//        << FMPAttribute::get_key_value("ttStrikeTech", event.attrs.at(2)) << ", "
//        << FMPAttribute::get_key_value("ttSpinKind", event.attrs.at(3)) << ")";

    out << "("
        << event.attrs.at(0) << ", "
        << event.attrs.at(1) << ", "
        << event.attrs.at(2) << ", "
        << event.attrs.at(3) << ")";
    return out;
}
