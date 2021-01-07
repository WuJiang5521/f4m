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
    usg = 0;
}

bool Pattern::codeTableSetComp::operator()(const Pattern* const &a, const Pattern* const &b) const {
    if (a->size() == b->size()) {
        return a->timeStamp < b->timeStamp;
    }
    return a->size() > b->size();
}

int Pattern::getUsage() const {
    return usg;
}

int Pattern::getGaps() const {
    return gaps;
}

int Pattern::getFills() const {
    return fills;
}

void Pattern::clearCnts() {
    usg = 0;
    gaps = 0;
    fills = 0;
}

std::vector<Event>::iterator Pattern::begin() {
    return pattern.begin();
}
std::vector<Event>::iterator Pattern::end() {
    return pattern.end();
}