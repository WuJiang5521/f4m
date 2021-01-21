#ifndef MULTIEVENT_H
#define MULTIEVENT_H

#include "stdafx.h"
#include "Event.h"
class DittoPattern;

using namespace std;

class Multi_event
{
	public: 
		const int alphabetSize;		//possible number of symbols in this Multi_event-event
		const int id;					//time step in the data sequence
		const int seqid;				//id of the sequence it is in
	
		Multi_event(int alphabetSize, int id, int seqid);
		~Multi_event();

		void finished();
		
		void print() const;
	
		int getSize() { return size; }
		eventSet* getEvents() const { return events; }	

		void addEvent(Event *e) { events->insert(e); size++;}
		void setNext(Multi_event * mev) { nxt = mev; };
		const Multi_event* next() const { return nxt; }						//next multi_event
		void resetCover();	
		bool cover(Event *e, DittoPattern *p);	//return true when entire Multi_event is covered
		bool tryCover(Event *e);			//return true when cover is possible
		DittoPattern* testCovered(int id) { return isCovered[id]; }

		
	private:
		Multi_event* nxt;		//a pointer to the next Multi_event in the data
		DittoPattern **isCovered;	//for each symbol by which pattern it is covered
		int	eventsCovered;		//counts nr of covered events so we can quickly see when entire Multi_event is covered
		int size;				//nr of events in this multi_event
				
		eventSet *events;		//set of event pointers for this Multi_event
};


inline bool operator==(const Multi_event& lhs, const Multi_event& rhs)
{
	return *lhs.getEvents() == *rhs.getEvents();
}



#endif