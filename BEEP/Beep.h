#ifndef BEEP_H
#define BEEP_H

#include "Common.h"
#include "Sequence.h"
#include "Pattern.h"
#include "CodeTable.h"
#include "Cover.h"
#include "Node.h"
#include "GetOpt.h"

class Beep {
public:
    explicit Beep(Parameters *parameters);

    ~Beep();

    Pattern *build_pattern(Pattern *a, Pattern *b, int offset);

    Pattern *build_interleaved_pattern(Pattern *singleton, Pattern *p, int pos_singleton);

    bool check_pattern_attribute_overlap(Pattern *a, Pattern *b, int offset);

    bool check_eventset_attribute_overlap(event_set *a, event_set *b);

    bool check_subset(Pattern *a, Pattern *b);

    bool check_subset(event_set *esa, event_set *esb);

    bool check_if_dummy(Pattern *p);

    bool combine_patterns(Pattern *a, Pattern *b, int total_usage, pattern_set *result);

    event_set *join_eventsets(event_set *a, event_set *b);

    void insert_candidates(pattern_set *list);

    usg_sz *postprune(Pattern *accepted, int total_usg, double new_size);

    pattern_set::iterator find_pattern_in_set(pattern_set *pset, Pattern *p);

    candpattern_set::iterator find_pattern_in_set(candpattern_set *pset, Pattern *p);

    prunepattern_set::iterator find_pattern_in_set(prunepattern_set *pset, Pattern *p);

    void break_down(Pattern *p);
#ifdef LSH
    void generate_candidates(usg_sz *current_usgSz); // our generator for less generation
#else
    void generate_candidates(usagepattern_set::iterator *pt_ct_1, usagepattern_set::iterator *pt_ct_2,
                             usagepattern_set::iterator *pt_begin_ct, usagepattern_set::iterator *pt_end_ct,
                             usg_sz *current_usgSz);
#endif
    usg_sz *try_variations(Pattern *accepted, usg_sz *current_usgSz);

    void load_or_build_min_windows(Pattern *p);

    CodeTable *get_codeTable() { return ct; }

    Cover *get_cover() { return cover; }


private:
    CodeTable *ct;

    candpattern_set *cand;            //ordered on estimated gain
    usagepattern_set *ct_on_usg;        //ordered on usage

    Cover *cover{};
#ifdef LSH
    priority_queue<pair<int, pair<Pattern*, Pattern*>>> *candidate_order; // candidate order that generated from Table
    const double cand_threshold = 0.05; // the candidate whose table value is less than cand_threshold * nrSequence is abandoned
#endif

    pattern_set *white_list;            //contains all considered candidates so we don't need to compute their support and min_windows again
    pattern_set *black_list;            //contains all considered candidates with sup = 0

    ostringstream output_stream;

    Node *root;

    Parameters *par;
};

int beep_enter(int argc, char **argv);

#endif