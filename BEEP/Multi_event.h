#ifndef MULTIEVENT_H
#define MULTIEVENT_H

#include "stdafx.h"
#include "Event.h"

class Pattern;

using namespace std;

class Multi_event {
public:
    const int alphabetSize;        //possible number of symbols in this Multi_event-event
    const int id;                    //time step in the data sequence
    const int seqid;                //id of the sequence it is in

    Multi_event(int alphabetSize, int id, int seqid);

    ~Multi_event();

    void finished();

    void print() const;

    int get_size() const { return size; }

    event_set *get_events() const { return events; }

    void add_event(Event *e) {
        events->insert(e);
        size++;
    }

    void set_next(Multi_event *mev) { nxt = mev; };

    const Multi_event *next() const { return nxt; }                        //next multi_event
    void reset_cover();

    bool cover(Event *e, Pattern *p);    //return true when entire Multi_event is covered
    bool try_cover(Event *e);            //return true when cover is possible
    Pattern *test_covered(int id) { return is_covered[id]; }


private:
    Multi_event *nxt;        //a pointer to the next Multi_event in the data
    Pattern **is_covered;    //for each symbol by which pattern it is covered
    int events_covered;        //counts nr of covered events so we can quickly see when entire Multi_event is covered
    int size;                //nr of events in this multi_event

    event_set *events;        //set of event pointers for this Multi_event
};


inline bool operator==(const Multi_event &lhs, const Multi_event &rhs) {
    return *lhs.get_events() == *rhs.get_events();
}


#endif