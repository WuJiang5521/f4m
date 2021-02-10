#ifndef EVENT_H
#define EVENT_H

#include "stdafx.h"
#include "Window.h"

using namespace std;

class Event {
public:
    Event(int s, int aid, int id, int tree_id) : symbol(s), attribute(aid), id(id), tree_id(tree_id) {}

    void print() const {
#ifdef MISS
        outfile_miss << "id: " << id << " sym: " << symbol << " aid: " << attribute << endl;
#else
        cout << "id: " << id << " sym: " << symbol << " aid: " << attribute << endl;
#endif
    }

    const int symbol;            //always 0 in ITEM SET data
    const int attribute;
    const int id;                //range [0,|multi_event|-1], to identify all events within a multi_event/pattern, e.g. used for isCovered
    const int tree_id;            //ranges from 1 to alphabetSize

};


inline bool operator<(const Event &lhs, const Event &rhs) {
    return lhs.tree_id < rhs.tree_id;
}

inline bool operator>(const Event &lhs, const Event &rhs) { return rhs < lhs; }

inline bool operator<=(const Event &lhs, const Event &rhs) { return !(lhs > rhs); }

inline bool operator>=(const Event &lhs, const Event &rhs) { return !(lhs < rhs); }

inline bool operator==(const Event &lhs, const Event &rhs) { return (!(lhs < rhs) && !(rhs < lhs)); }

inline bool operator!=(const Event &lhs, const Event &rhs) { return !(lhs == rhs); }

struct EventPtrComp {
    bool operator()(const Event *lhs, const Event *rhs) const {
        return *lhs < *rhs;
    }
};

typedef std::set<Event *, EventPtrComp> eventSet;                    //set of events



#endif