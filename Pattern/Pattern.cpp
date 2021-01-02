//
// Created by A on 2020/12/22.
//

#include "Pattern.hpp"

#include <utility>

using namespace std;

Event& Pattern::operator[](int i) {
    return pattern[i];
}


const Event& Pattern::operator[](int i) const {
    return pattern[i];
}

int Pattern::size() const {
    return pattern.size();
}

Pattern::Pattern(std::vector<Event>  segments) : pattern(move(segments)) {
    timeStamp = totalTimeStamp++;
}

bool Pattern::codeTableSetComp::operator()(const Pattern &a, const Pattern &b) const {
    if (a.size() == b.size()) {
        return a.timeStamp < b.timeStamp;
    }
    return a.size() > b.size();
}