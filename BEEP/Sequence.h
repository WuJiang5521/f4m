#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Common.h"
#include "Event.h"
#include "MathUtil.h"
#include "Pattern.h"

//#define LSH
#ifdef LSH
#include "PatternTable.h"
#endif

using namespace std;

class Sequence {
public:
    Sequence(FILE *f, Parameters *par);    //constructor
    ~Sequence();

    void pre_init();

    void init();

    int read(FILE *f);

    Event **get_mev_time() { return mev_time; }

    float get_mev_size_term(int id) { return mev_term[mev_time[id]->get_size()]; }

    MathUtil *get_mu() const { return mu; }

    int get_input_type() const { return par->input_type; }

    Parameters *get_parameters() { return par; }

    int get_nr_events() const { return nr_events; }

    int get_nr_sequences() const { return nr_sequences; }

    int *get_sequence_sizes() const { return sequence_sizes; }

    double **get_st_codelengths() { return ST_codelengths; }

    int get_sup(int aid, int s) { return occ[aid][s].size(); }

    int **get_tree_ids() { return tree_ids; }

    void compute_st_codelengths();

    string print_sequence(bool all_values);

    const list<Event *> *find_occurrences(event_set *events) const;

    int load_dummies();

    void reset_cover() {
        for (int i = 0; i < par->nr_events; ++i) {
            is_covered[i] = 0;
            mev_time[i]->reset_cover();
        }
        mev_covered = 0;
    }

    bool cover(Pattern *p, Window *w); //pos = id of event in the sequence
    bool cover(Attribute *e, int pos, Pattern *p);
#ifdef MISS
    int try_cover(event_set *events, int pos); // number of miss events number
#else
    bool try_cover(event_set *events, int pos); //true = cover is possible
#endif
    bool try_cover(Attribute *e, int pos);

    void cover_singletons(Pattern ***singletons);

    bool g_debug;//DEBUG
    bool g_error;
#ifdef LSH
    set<Pattern*> *cover_pattern; // cover_pattern[i] includes the patterns emerge in sequence i
    const int cut_size = 50; // a sequence's length is no longer than cut_size
#endif

protected:
    Parameters *par;

    MathUtil *mu;

    Event **mev_time;                        //a multi-event array, one event pointer per time step

    list<Event *> **occ;                        //for each attribute for each symbol an array of Event* where it occurs,
    //CATEGORICAL: occ[aid][s] -> list of Event* for symbol s on attribute aid
    //ITEM SET:	   occ[a][0]   -> list of Event* for attribute a (each attribute can have only one value)

    int nr_events;                                    //total number of events
    int nr_sequences;                                //number of sequences in the data
    int *sequence_sizes;                            //holds the length of each sequence (sequences are separated with '-1' in the input)
    double **ST_codelengths;                        //for each attribute for each symbol its base codelengths, for itemset data ST_codelengths[aid][s], with aid=0

    int **tree_ids;                                    //all tree-id's based on attr and sym

    int *is_covered;                                    //for every event whether it is completely covered. E.g. is_covered[timestep] = 1 means it is completely covered
    int mev_covered;                                    //the number of completely covered events

    float *mev_term;                                //for each event on position ' id'  with 'sz' events: -lg(probMev[id]) = -lg(P(sz | data)) = -log of the probability that a event in the data has 'sz' events

    ostringstream output_stream;
};


#endif
