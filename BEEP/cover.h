#ifndef COVER_H
#define COVER_H

#include "stdafx.h"
#include "Pattern.h"
#include "Sequence.h"
#include "CodeTable.h"
#include "P_P_Table.h"
//#ifndef LSH
//#define LSH
//#endif
using namespace std;

class Cover {
private:
    CodeTable *g_codeTable;
    Sequence *g_sequence;
    int g_total_usage;
#ifdef MISS
    int g_total_usage_miss; // total usage for miss code
#endif
    double g_sz_sequence_and_CT;            //total compressed size of Data and Code Table: L(D, CT) = L(D|CT) + L(CT)
    bool other_data;                    //false = we cover the data on which the Code Table is build, true = we cover other data

    Parameters *par;

public:
    Cover(Sequence *s, CodeTable *ct, bool other_data);

    ~Cover();

    void compute_total_usage(codeTable_set *ct);

#ifdef MISS
    void compute_total_usage_miss(codeTable_set *ct);
#endif

    bool cover_with_pattern_min_windows(Pattern *p);
#ifdef MISS
    int cover_window_with_pattern(Window *w, Pattern *p);
#else
    bool cover_window_with_pattern(Window *w, Pattern *p);
#endif

    void update_pattern_codelengths(
            codeTable_set *ct);    //after every Cover we need to update the codelenghts, except when we use a code set to cover other/new data

    double get_sz_sequence_and_CT() const { return g_sz_sequence_and_CT; }

    int get_total_usage() const { return g_total_usage; }
};

#endif
