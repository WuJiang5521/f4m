#ifndef WINDOW_H
#define WINDOW_H

#include "stdafx.h"

class DittoPattern;

class Multi_event;

using namespace std;

class Window {
public:
    Window(const Multi_event **mevp, DittoPattern *p);

    ~Window();

    const Multi_event *get_mev_position(int id) { return mev_positions[id]; }

    int get_GapLength() const { return gapLength; }

    DittoPattern *get_DittoPattern() { return pat; }

    set<int> *get_gaps() { return gaps; }

    bool equal(Window *rhs) const;

    void print() const;

    Window *nextDisjoint;                //the next disjoint minimal window for this pattern
    Window *prevDisjoint;                //the previous disjoint minimal window for this pattern
    Window *next;                        //the next minimal window for this pattern
    const Multi_event *first, *last;    //the start and end of the minimal window
    bool active;                        //true=used in cover, false=not used

private:
    DittoPattern *pat;                        //for which pattern this is a minimal window
    const Multi_event **mev_positions;    //for each time step in the pattern a pointer to the corresponding multi_event for this window
    set<int> *gaps;                        //set of timesteps (ID's of Multi_events) where there is a gap
    int gapLength;                        //total gap length when covering this window with p
};


#endif