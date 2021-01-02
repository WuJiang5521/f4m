//
// Created by A on 2020/12/22.
//

#include "Sequence.hpp"

using namespace std;

Sequence::Sequence(vector<Event> events) :
        events(move(events)) {
    cover.resize(this->events.size());
    for (int i = 0; i < cover.size(); ++i) {
        cover[i].resize(this->events[i].size(), make_pair(nullptr, -1));
    }
}

ostream &operator<<(ostream &out, const Sequence &sequence) {
    for (auto i = sequence.events.begin(); i != sequence.events.end(); i++)
        out << (i == sequence.events.begin() ? "" : ", ") << *i;
    return out;
}

void Sequence::getOccur(std::set<std::pair<int, std::vector<int> >, occurSetComp> & occurs, const Pattern & X) {
    for (int i = 0; i < events.size(); ++i) {
        std::vector<int> aCover = {};
        int j = i, xId = 0, countGap = 0;
        while (countGap < X.size() && j < events.size() && xId < X.size()) {
            if (events[j] == X[xId]) {
                aCover.push_back(xId);
                j++;
                xId++;
            } else {
                aCover.push_back(coverMissFlag);
                j++;
                countGap++;
            }
        }
        if (xId == X.size()) {
            occurs.insert(make_pair(i, aCover));
        }
    }
}

bool Sequence::hasInterSection(const std::pair<int, std::vector<int> > & occur, const Pattern & X) const {
    int occurAt = occur.first;
    for (auto it = occur.second.begin(); it != occur.second.begin(); ++it, occurAt++) {
        if (*it == coverMissFlag) {
            continue;
        }
        for (int j = 0; j < cover[occurAt].size(); ++j) {
            if (X[*it][j] != Pattern::patternNULLFlag && cover[occurAt][j].first != nullptr) {
                return true;
            }
        }
    }
    return false;
}

bool Sequence::coverIsFull() const {
    for (const auto & i : cover) {
        for (const auto & j : i) {
            if (j.first == nullptr) {
                return false;
            }
        }
    }
    return true;
}

void Sequence::getCover(set<Pattern, Pattern::codeTableSetComp> &PS) {
    int pi = 0;
    auto it = PS.begin();
    for (; it != PS.end(); ++it, pi++) {
        set<pair<int, vector<int> >, occurSetComp> occurs;
        getOccur(occurs, *it);
        for (const auto & occur : occurs) {
            if (!hasInterSection(occur, *it)) {
                int occurAt = occur.first;
                for (auto occurIt = occur.second.begin(); occurIt != occur.second.end(); ++occurIt, occurAt++) {
                    if (*occurIt == coverMissFlag) {
                        continue;
                    }
                    for (int j = 0; j < cover[occurAt].size(); ++j) {
                        if ((*it)[*occurIt][j] != Pattern::patternNULLFlag) {
                            cover[occurAt][j].first = &(*it);
                            cover[occurAt][j].second = *occurIt;
                        }
                    }
                }
            }
        }
        if (coverIsFull()) break;
    }
}

bool Sequence::occurSetComp::operator()(const std::pair<int, std::vector<int> > &a,
                                        const std::pair<int, std::vector<int> > &b) {
    return a.second.size() < b.second.size();
}

int Sequence::size() const {
    return events.size();
}