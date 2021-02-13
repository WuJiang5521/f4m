#ifndef CODETABLE_H
#define CODETABLE_H

#include "stdafx.h"
#include "Pattern.h"
#include "Sequence.h"
#include "mathutil.h"

using namespace std;

class CodeTable {
public:
    explicit CodeTable(Sequence *s);

    ~CodeTable();

    double compute_szD_CT(Sequence *s);

    double compute_szCT_C(Sequence *s);

    double
    compute_sz(Sequence *s);                            //used when we use the CodeTable to compress other data

    void update_ratio(double rt) { ratio = rt; };

    void delete_unused_patterns();                                //NOTE: only used at the very end
    void delete_pattern(Pattern *p);

    bool insert_pattern(Pattern *p);

    void rollback();

    string print_CT() const { return print_CT(true); };

    string print_CT(bool console_output) const;

    string printpattern_set(bool console_output, codeTable_set *plist, const string &title) const;

    string printcandidate_set(bool console_output, candpattern_set *plist, const string &title) const;

    double **get_STcodelengths() const { return g_STcodelengths; };

    int get_CT_length() const { return length_CT; };

    codeTable_set *get_CT() const { return g_codeTable; };
#ifdef MISS
    mathutil *get_math_util() const { return mu; }
#endif

    void set_szCT_C(Sequence *s) {
        szCT_C = compute_szCT_C(s);
    }; //only called when the final CT for this data is computed

    list<usg_sz> *get_nr_non_singletons_per_size();

private:
    Parameters *par;
    mathutil *mu;
    codeTable_set *g_codeTable;            //COVER ORDER: descending on cardinality, descending on support, descending on L(X|ST), ascending on AID-rank, lexicographically ascending

    Sequence *g_sequence;            //the data

    int length_CT;                //total number of patterns in the codeTable (incl singletons)
    double ratio;                    //between current compression rate of current codeSet versus standard codeTable

    double **g_STcodelengths;        //FOR EACH idLevel: the code length for each singleton item based on its relative frequency in the data (indexed by its own item-value, i.e. the length of pattern 2 is stored at position 2)
    double szCT_C;                    //only computed once when the CT is used to cover the data it is build on
};

#endif
