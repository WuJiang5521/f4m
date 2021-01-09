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
//Sequence & Sequence::operator=(const Sequence & a) {
//    for (int i = 0; i < cover.size(); ++i) {
//        for (int j = 0; j < cover[i].size(); ++j) {
//            cover[i][j].first = a.cover[i][j].first;
//            cover[i][j].second = a.cover[i][j].second;
//        }
//    }
//    return *this;
//}

vector<Event>::iterator Sequence::begin() {
    return events.begin();
}
vector<Event>::iterator Sequence::end() {
    return events.end();
}

void Sequence::getOccur(OccursType & occurs, const Pattern & X) const {
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
            occurs.insert(make_pair(make_pair(i, countGap), aCover));
        }
    }
}

bool Sequence::hasInterSection(const OccurType & occur, const Pattern & X) const {
    int occurAt = occur.first.first;
    for (auto it = occur.second.begin(); it != occur.second.end(); ++it, occurAt++) {
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

void Sequence::getCover(CodeTableType &PS) {
    coverPattern.clear();
    for (int i = 0; i < cover.size(); ++i) {
        for (int j = 0; j < cover[i].size(); ++j) {
            cover[i][j].first = nullptr;
            cover[i][j].second = -1;
        }
    }
    int pi = 0;
    auto it = PS.begin();
    for (; it != PS.end(); ++it, pi++) {
        if ((*it)->disabled) {
            continue;
        }
        OccursType occurs = {};
        getOccur(occurs, **it);
        bool counted = false;
        for (const auto & occur : occurs) {
            if (!hasInterSection(occur, **it)) {
                int occurAt = occur.first.first;
                int gaps = occur.first.second;
                for (auto occurIt = occur.second.begin(); occurIt != occur.second.end(); ++occurIt, occurAt++) {
                    if (*occurIt == coverMissFlag) {
                        continue;
                    }
                    for (int j = 0; j < cover[occurAt].size(); ++j) {
                        if ((**it)[*occurIt][j] != Pattern::patternNULLFlag) {
                            cover[occurAt][j].first = &(**it);
                            cover[occurAt][j].second = *occurIt;
                        }
                    }
                }
                (*it)->gaps += gaps;
                (*it)->fills += (*it)->size() - 1;
                (*it)->usg++;
                if (coverPattern.find(*it) != coverPattern.end()) {
                    coverPattern[*it].push_back(occurAt);
                } else {
                    coverPattern[*it] = {occurAt};
                }
                if (!counted) {
                    counted = true;
                    for (auto & p : coverPattern) {
                        if (p.first == *it) {
                            continue;
                        }
                        int minDis = accumulate(p.second.begin(), p.second.end(), 100000, [occurAt](int minn, int a) {
                           int absul = occurAt - a > 0 ? occurAt - a : a - occurAt;
                           return minn > absul ? absul : minn;
                        });
                        if (minDis <= p.first->size() + (*it)->size()) {
                            if (P_PTable::table[p.first].find(*it) != P_PTable::table[p.first].end()) {
                                P_PTable::table[p.first][*it]++;
                            }
                            if (P_PTable::table[*it].find(p.first) != P_PTable::table[*it].end()) {
                                P_PTable::table[*it][p.first]++;
                            }
                        }
                    }
                }
            }
        }
        if (coverIsFull()) break;
    }
}

bool Sequence::occurSetComp::operator()(const OccurType &a,
                                        const OccurType &b) {
    if (a.second.size() == b.second.size()) {
        return a.first.first < b.first.first;
    }
    return a.second.size() < b.second.size();
}

int Sequence::size() const {
    return events.size();
}