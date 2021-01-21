//
// Created by A on 2020/12/22.
//

#include "FmpPattern.hpp"

#include <utility>

using namespace std;

BaseEvent& FmpPattern::operator[](int i) {
    return pattern[i];
}


const BaseEvent& FmpPattern::operator[](int i) const {
    return pattern[i];
}

int FmpPattern::size() const {
    return pattern.size();
}

FmpPattern::FmpPattern(std::vector<BaseEvent>  segments) : pattern(move(segments)) {
    timeStamp = totalTimeStamp++;
    usg = 0;
    gaps = 0;
    fills = 0;
    oldUsg = 0;
    oldGaps = 0;
    oldFills = 0;
    disabled = false;
}

bool FmpPattern::codeTableSetComp::operator()(const FmpPattern* const &a, const FmpPattern* const &b) const {
    if (a->size() == b->size()) {
        return a->timeStamp < b->timeStamp;
    }
    return a->size() > b->size();
}

int FmpPattern::getUsage() const {
    return usg;
}

int FmpPattern::getGaps() const {
    return gaps;
}

int FmpPattern::getFills() const {
    return fills;
}

void FmpPattern::rollback() {
    usg = oldUsg;
    gaps = oldGaps;
    fills = oldFills;
}

void FmpPattern::checkup() {
    oldUsg = usg;
    oldGaps = gaps;
    oldFills = fills;
}


void FmpPattern::clearCnts() {
    usg = 0;
    gaps = 0;
    fills = 0;
}

std::vector<BaseEvent>::iterator FmpPattern::begin() {
    return pattern.begin();
}
std::vector<BaseEvent>::iterator FmpPattern::end() {
    return pattern.end();
}

void FmpPattern::push_back(const BaseEvent & e) {
    pattern.push_back(e);
}