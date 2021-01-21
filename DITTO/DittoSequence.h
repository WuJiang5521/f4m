#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "stdafx.h"
#include "Multi_event.h"
#include "mathutil.h"
#include "DittoPattern.h"

using namespace std;

class DittoSequence {
	public:
		DittoSequence(FILE *f, Parameters *par);	//constructor
		~DittoSequence();

		void pre_init();
		void init();

		int read(FILE *f);

		Multi_event** get_mev_time() { return g_mev_time; }
		float get_mev_size_term(int id) { return g_mevTerm[g_mev_time[id]->getSize()]; }
		mathutil* getMu() const { return mu; }
		int getInputType() const { return par->input_type; }
		
		Parameters* getParameters() { return par; }
		int get_nrEvents() const { return g_nrEvents; }			
		int get_nrSequences() const { return g_nrSequences; }
		int* get_sequence_sizes() const { return g_sequence_sizes; }
		double** get_STcodelengths() { return g_STcodelengths; }
		int getSup(int aid, int s) { return g_occ[aid][s].size(); }
		int** get_tree_ids() { return tree_ids; }

		void computeSTCodelengths();
		string print_sequence(bool allValues);
		const list<Multi_event*>* findOccurrences(eventSet *events) const;

		int load_dummies();

		void resetCover() { for (int i = 0; i < par->nrMulti_events; ++i) { isCovered[i] = 0; g_mev_time[i]->resetCover(); } mevCovered = 0; }		
		bool cover(DittoPattern *p, Window *w); //pos = id of multi_event in the sequence
		bool cover(Event *e, int pos, DittoPattern *p);
		bool tryCover(eventSet *events, int pos); //true = cover is possible
		bool tryCover(Event *e, int pos);
		void coverSingletons(DittoPattern*** singletons);

		bool g_debug;//DEBUG
		bool g_error;

	protected:
		Parameters *par;

		mathutil *mu;
		
		Multi_event** g_mev_time;						//a multi-event array, one multi_event pointer per time step
	
		list<Multi_event*>** g_occ;						//for each attribute for each symbol an array of Multi_event* where it occurs, 
														//CATEGORICAL: g_occ[aid][s] -> list of Multi_event* for symbol s on attribute aid
														//ITEM SET:	   g_occ[a][0]   -> list of Multi_event* for attribute a (each attribute can have only one value)

		int g_nrEvents;									//total number of events
		int g_nrSequences;								//number of sequences in the data
		int *g_sequence_sizes;							//holds the length of each sequence (sequences are separated with '-1' in the input)
		double **g_STcodelengths;						//for each attribute for each symbol its base codelengths, for itemset data g_STcodelengths[aid][s], with aid=0

		int** tree_ids;									//all tree-id's based on attr and sym

		int *isCovered;									//for every multi_event whether it is completely covered. E.g. isCovered[timestep] = 1 means it is completely covered
		int mevCovered;									//the number of completely covered Multi_events

		float* g_mevTerm;								//for each multi_event on position ' id'  with 'sz' events: -lg(probMev[id]) = -lg(P(sz | data)) = -log of the probability that a multi_event in the data has 'sz' events

		ostringstream	g_outputStream;
};


#endif
