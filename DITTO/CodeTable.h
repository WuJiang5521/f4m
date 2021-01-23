#ifndef CODETABLE_H
#define CODETABLE_H

#include "stdafx.h"
#include "DittoPattern.h"
#include "DittoSequence.h"
#include "mathutil.h"

using namespace std;

class CodeTable {
public:
    explicit CodeTable(DittoSequence *s);

    ~CodeTable();

    double compute_szD_CT(DittoSequence *s);

    double compute_szCT_C(DittoSequence *s);

    double
    compute_sz(DittoSequence *s);                            //used when we use the CodeTable to compress other data

    void updateRatio(double rt) { ratio = rt; };

    void deleteUnusedDittoPatterns();                                //NOTE: only used at the very end
    void deleteDittoPattern(DittoPattern *p);

    bool insertDittoPattern(DittoPattern *p);

    void rollback();

    string printCT() const { return printCT(true); };

    string printCT(bool consoleOutput) const;

    string printpatternSet(bool consoleOutput, codeTableSet *plist, const string &title) const;

    string printcandidateSet(bool consoleOutput, candpatternSet *plist, const string &title) const;

    double **get_STcodelengths() const { return g_STcodelengths; };

    int getCTLength() const { return length_CT; };

    codeTableSet *getCT() const { return g_codeTable; };

    void set_szCT_C(DittoSequence *s) {
        szCT_C = compute_szCT_C(s);
    }; //only called when the final CT for this data is computed

    list<usgSz> *get_nr_non_singletons_per_size();

private:
    Parameters *par;
    mathutil *mu;
    codeTableSet *g_codeTable;            //COVER ORDER: descending on cardinality, descending on support, descending on L(X|ST), ascending on AID-rank, lexicographically ascending

    DittoSequence *g_sequence;            //the data

    int length_CT;                //total number of patterns in the codeTable (incl singletons)
    double ratio;                    //between current compression rate of current codeSet versus standard codeTable

    double **g_STcodelengths;        //FOR EACH idLevel: the code length for each singleton item based on its relative frequency in the data (indexed by its own item-value, i.e. the length of pattern 2 is stored at position 2)
    double szCT_C;                    //only computed once when the CT is used to cover the data it is build on
};

#endif
