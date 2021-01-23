#include "stdafx.h"
#include "Multi_event.h"

using namespace std;


Multi_event::Multi_event(int alphabetSize, int id, int seqid) : alphabetSize(alphabetSize), id(id), seqid(seqid) {
    events = new eventSet;
    nxt = nullptr;
    size = 0;
}

Multi_event::~Multi_event() {
    events->clear();            //delete the events
    delete events;                //delete the container
    delete[]isCovered;
}


//when all symbols are added to this Multi_event
void Multi_event::finished() {
    isCovered = new DittoPattern *[size];
}

void Multi_event::print() const {
    std::cout << "Multi_event: alphabetSize = " << alphabetSize << " sym-aid-id {";
    auto it = events->begin(), end = events->end();
    while (it != end) {
        std::cout << (*it)->symbol << "-" << (*it)->attribute << "-" << (*it)->id;
        if (++it != end)
            std::cout << ", ";
    }
    std::cout << "} id: " << id << " seqid: " << seqid << std::endl;
}

void Multi_event::resetCover() {
    for (int i = 0; i < size; ++i)
        isCovered[i] = nullptr;
    eventsCovered = 0;
}

//return true when entire Multi_event is covered
bool Multi_event::cover(Event *e, DittoPattern *p) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!isCovered[(*it)->id]) {
            isCovered[(*it)->id] = p;
            eventsCovered++;
        }
    }
    return (eventsCovered == size);
}

//return true when cover is possible
bool Multi_event::tryCover(Event *e) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!isCovered[(*it)->id])
            return true;
    }
    return false;
}	

		