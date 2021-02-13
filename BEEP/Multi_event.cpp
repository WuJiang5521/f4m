#include "stdafx.h"
#include "Multi_event.h"

using namespace std;


Multi_event::Multi_event(int alphabetSize, int id, int seqid) : alphabetSize(alphabetSize), id(id), seqid(seqid) {
    events = new event_set;
    nxt = nullptr;
    size = 0;
}

Multi_event::~Multi_event() {
    events->clear();            //delete the events
    delete events;                //delete the container
    delete[]is_covered;
}


//when all symbols are added to this Multi_event
void Multi_event::finished() {
    is_covered = new Pattern *[size];
}

void Multi_event::print() const {
    std::cout << "Multi_event: alphabet_size = " << alphabetSize << " sym-aid-id {";
    auto it = events->begin(), end = events->end();
    while (it != end) {
        std::cout << (*it)->symbol << "-" << (*it)->attribute << "-" << (*it)->id;
        if (++it != end)
            std::cout << ", ";
    }
    std::cout << "} id: " << id << " seqid: " << seqid << std::endl;
}

void Multi_event::reset_cover() {
    for (int i = 0; i < size; ++i)
        is_covered[i] = nullptr;
    events_covered = 0;
}

//return true when entire Multi_event is covered
bool Multi_event::cover(Event *e, Pattern *p) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!is_covered[(*it)->id]) {
            is_covered[(*it)->id] = p;
            events_covered++;
        }
    }
    return (events_covered == size);
}

//return true when cover is possible
bool Multi_event::try_cover(Event *e) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!is_covered[(*it)->id])
            return true;
    }
    return false;
}	

		