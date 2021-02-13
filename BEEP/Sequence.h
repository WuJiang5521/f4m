#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "stdafx.h"
#include "Multi_event.h"
#include "mathutil.h"
#include "Pattern.h"

//#define LSH
#ifdef LSH
#include "P_P_Table.h"
#endif

using namespace std;

class Sequence {
public:
    Sequence(FILE *f, Parameters *par);    //constructor
    ~Sequence();

    void pre_init();

    void init();

    int read(FILE *f);

    Multi_event **get_mev_time() { return g_mev_time; }

    float get_mev_size_term(int id) { return g_mev_term[g_mev_time[id]->get_size()]; }

    mathutil *get_mu() const { return mu; }

    int get_input_type() const { return par->input_type; }

    Parameters *get_parameters() { return par; }

    int get_nr_events() const { return g_nr_events; }

    int get_nr_sequences() const { return g_nr_sequences; }

    int *get_sequence_sizes() const { return g_sequence_sizes; }

    double **get_ST_codelengths() { return g_ST_codelengths; }

    int getSup(int aid, int s) { return g_occ[aid][s].size(); }

    int **get_tree_ids() { return tree_ids; }

    void compute_ST_codelengths();

    string print_sequence(bool all_values);

    const list<Multi_event *> *find_occurrences(event_set *events) const;

    int load_dummies();

    void reset_cover() {
        for (int i = 0; i < par->nr_multi_events; ++i) {
            is_covered[i] = 0;
            g_mev_time[i]->reset_cover();
        }
        mev_covered = 0;
    }

    bool cover(Pattern *p, Window *w); //pos = id of multi_event in the sequence
    bool cover(Event *e, int pos, Pattern *p);
#ifdef MISS
    int try_cover(event_set *events, int pos); // number of miss events number
#else
    bool try_cover(event_set *events, int pos); //true = cover is possible
#endif
    bool try_cover(Event *e, int pos);

    void cover_singletons(Pattern ***singletons);

    bool g_debug;//DEBUG
    bool g_error;
#ifdef LSH
    set<Pattern*> *cover_pattern; // cover_pattern[i] includes the patterns emerge in sequence i
    const int cut_size = 50; // a sequence's length is no longer than cut_size
#endif

protected:
    Parameters *par;

    mathutil *mu;

    Multi_event **g_mev_time;                        //a multi-event array, one multi_event pointer per time step

    list<Multi_event *> **g_occ;                        //for each attribute for each symbol an array of Multi_event* where it occurs,
    //CATEGORICAL: g_occ[aid][s] -> list of Multi_event* for symbol s on attribute aid
    //ITEM SET:	   g_occ[a][0]   -> list of Multi_event* for attribute a (each attribute can have only one value)

    int g_nr_events;                                    //total number of events
    int g_nr_sequences;                                //number of sequences in the data
    int *g_sequence_sizes;                            //holds the length of each sequence (sequences are separated with '-1' in the input)
    double **g_ST_codelengths;                        //for each attribute for each symbol its base codelengths, for itemset data g_ST_codelengths[aid][s], with aid=0

    int **tree_ids;                                    //all tree-id's based on attr and sym

    int *is_covered;                                    //for every multi_event whether it is completely covered. E.g. is_covered[timestep] = 1 means it is completely covered
    int mev_covered;                                    //the number of completely covered Multi_events

    float *g_mev_term;                                //for each multi_event on position ' id'  with 'sz' events: -lg(probMev[id]) = -lg(P(sz | data)) = -log of the probability that a multi_event in the data has 'sz' events

    ostringstream g_output_stream;
};


#endif
