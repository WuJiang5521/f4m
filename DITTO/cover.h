#ifndef COVER_H
#define COVER_H

#include "stdafx.h"
#include "DittoPattern.h"
#include "DittoSequence.h"
#include "CodeTable.h"

using namespace std;

class Cover {
private:
    CodeTable *g_codeTable;
    DittoSequence *g_sequence;

    int g_totalUsage;
    double g_szSequenceAndCT;            //total compressed size of Data and Code Table: L(D, CT) = L(D|CT) + L(CT)
    bool otherData;                    //false = we cover the data on which the Code Table is build, true = we cover other data

    Parameters *par;

public:
    Cover(DittoSequence *s, CodeTable *ct, bool otherData);

    ~Cover();

    void computeTotalUsage(codeTableSet *ct);

    bool coverWithDittoPatternMinWindows(DittoPattern *p);

    bool coverWindowWithDittoPattern(Window *w, DittoPattern *p);

    void updateDittoPatternCodelengths(
            codeTableSet *ct);    //after every Cover we need to update the codelenghts, except when we use a code set to cover other/new data

    double get_szSequenceAndCT() const { return g_szSequenceAndCT; }

    int get_totalUsage() const { return g_totalUsage; }
};

#endif
