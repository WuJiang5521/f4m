#ifndef EVENT_H
#define EVENT_H

#include "Common.h"
#include "Attribute.h"

class Pattern;

using namespace std;

class Event {
public:
    const int alphabetSize;        //possible number of symbols in this Event-event
    const int id;                    //time step in the data sequence
    const int seqid;                //id of the sequence it is in

    Event(int alphabetSize, int id, int seqid);

    ~Event();

    void finished();

    void print() const;

    int get_size() const { return size; }

    event_set *get_events() const { return events; }

    void add_event(Attribute *e) {
        events->insert(e);
        size++;
    }

    void set_next(Event *mev) { nxt = mev; };

    const Event *next() const { return nxt; }                        //next event
    void reset_cover();

    bool cover(Attribute *e, Pattern *p);    //return true when entire Event is covered
    bool try_cover(Attribute *e);            //return true when cover is possible
    Pattern *test_covered(int id) { return is_covered[id]; }


private:
    Event *nxt;        //a pointer to the next Event in the data
    Pattern **is_covered;    //for each symbol by which pattern it is covered
    int events_covered;        //counts nr of covered events so we can quickly see when entire Event is covered
    int size;                //nr of events in this event

    event_set *events;        //set of event pointers for this Event
};


inline bool operator==(const Event &lhs, const Event &rhs) {
    return *lhs.get_events() == *rhs.get_events();
}


#endif