#include "stdafx.h"
#include "cover.h"
/*	Changes the usage of the DittoPatterns in the CodeTable as a result of the covering process
	bool otherData: false = loop through the block's occurences, true = try to fit block on all positions in data */
Cover::Cover(DittoSequence *s, CodeTable *codeTable, bool otherData) : g_sequence(s),
                                                                       g_codeTable(codeTable),
                                                                       otherData(otherData) {
    g_sequence->getParameters()->cntCovers++;
#ifdef FMP

    delete[] g_sequence->coverPattern;

    g_sequence->coverPattern = new set<DittoPattern*>[g_sequence->get_nrSequences()];
#endif

    g_totalUsage = 0;
    g_szSequenceAndCT = 0;
    g_sequence->resetCover();

    bool coverComplete = false;
    codeTableSet *ct = g_codeTable->getCT();
    DittoPattern *candidate;

    par = g_sequence->getParameters();

    //make an array to quickly get pointers to singletons based on their attribute and symbol
    int nrAttr = par->nrOfAttributes;
    int *alpSzs = par->alphabetSizes;

    auto ***singletons = new DittoPattern **[nrAttr];
    for (int atr = 0; atr < nrAttr; ++atr)
        singletons[atr] = new DittoPattern *[alpSzs[atr]];

    //Standard Cover Order
    for (auto it_blockList : *ct) {
        candidate = it_blockList;

        if (candidate->getSize() == 1) {
            //optimisation -> when arrived at singletons just loop once over data and cover all uncovered symbols with singletons
            coverComplete = true;                                        //to make sure the usages of all singletons are reset
            singletons[(*candidate->getSymbols(0)->begin())->attribute][(*candidate->getSymbols(
                    0)->begin())->symbol] = candidate;
        }

        candidate->resetUsage();                                        //reset the usage for the DittoPattern in the codeTable before covering
        if (coverComplete)
            continue;                                                    //do not break, because we still need to reset all usages

        coverComplete = coverWithDittoPatternMinWindows(candidate);
    }

    //cover the rest with singletons
    g_sequence->coverSingletons(singletons);
#ifdef FMP
    for (int seq_id = 0; seq_id < g_sequence->get_nrEvents() / g_sequence->cutSize; ++seq_id) {
        for (auto cp1 = g_sequence->coverPattern[seq_id].begin(); cp1 != g_sequence->coverPattern[seq_id].end(); ++cp1) {
            for (auto cp2 = cp1; cp2 != g_sequence->coverPattern[seq_id].end(); ++cp2) {
                (*P_PTable::table)[P_PTable::patternIDMap[*cp1]][P_PTable::patternIDMap[*cp2]]++;
            }
        }
    }
#endif
    for (int atr = 0; atr < nrAttr; ++atr)
        delete[]singletons[atr];
    delete[]singletons;

    computeTotalUsage(ct);                                                //excluding laplace
#ifdef MISS
    computeTotalUsageMiss(ct);
#endif
    updateDittoPatternCodelengths(
            ct);                                        //only changes CL when we otherData = false
    g_szSequenceAndCT = g_codeTable->compute_sz(g_sequence);
}

void Cover::computeTotalUsage(codeTableSet *ct) {
    g_totalUsage = 0;
    for (auto it_blockList : *ct)
        g_totalUsage += it_blockList->getUsage();
}

#ifdef MISS
void Cover::computeTotalUsageMiss(codeTableSet *ct) {
    g_totalUsageMiss = 0;
    for (auto it_blockList : *ct)
        g_totalUsageMiss += it_blockList->getUsageMiss();
}
#endif

//cover all minimal windows of the pattern (when not covered yet)
//RETURN true if the cover is complete afterwards
bool Cover::coverWithDittoPatternMinWindows(DittoPattern *p) {
    bool coverComplete = false;

    //loop through all this block's minimal windows
    list<Window *> *lst = p->getMinWindows(g_sequence, otherData);

    for (auto w : *lst) { //defines a minimal window
#ifdef MISS
        int missCnt;
        if ((missCnt = coverWindowWithDittoPattern(w, p)) != -1) {
#else
        if (coverWindowWithDittoPattern(w, p)) {
#endif
            //cover this window with pattern p (NOTE: we do this only AFTER having checked tryCover
            //TODO FUTURE: try if alternative of other patterns is cheaper
            w->active = true;
#ifdef MISS
            p->updateUsages(w->get_GapLength(), missCnt);
#else
            p->updateUsages(w->get_GapLength());
#endif
            coverComplete = g_sequence->cover(p, w);
#ifdef FMP
            int seq_id = w->get_mev_position(0)->seqid;
            g_sequence->coverPattern[seq_id].insert(p);
#endif
        }

        if (coverComplete)
            break;
    }
    return coverComplete;
}
#ifdef MISS
int Cover::coverWindowWithDittoPattern(Window *w, DittoPattern *p) {
#else
bool Cover::coverWindowWithDittoPattern(Window *w, DittoPattern *p) {
#endif
    //check if gap is not bigger than patternlength-1
    if (w->get_GapLength() > p->getLength() - 1)
#ifdef MISS
        return -1;
#else
        return false;
#endif
#ifdef MISS
    int missCnt = 0;
#endif
    //for each timestep in the pattern test whether the data can still be covered for this window
    for (int ts = 0; ts < p->getLength(); ++ts) {
#ifdef MISS
        missCnt += g_sequence->tryCover(p->getSymbols(ts), w->get_mev_position(ts)->id);
        if (missCnt > (p->getSize() + 5) / 10) {
            return -1;
        }
#else
        if (!g_sequence->tryCover(p->getSymbols(ts), w->get_mev_position(ts)->id))
            return false;
#endif
    }
#ifdef MISS
    return missCnt;
#else
    return true;
#endif
}


void Cover::updateDittoPatternCodelengths(codeTableSet *ct) {
    if (!otherData) {
        double sum = (double) g_totalUsage + g_codeTable->getCTLength() * laplace;        //including laplace
#ifdef MISS
        double missSum = (double) g_totalUsageMiss;
#endif
        //loop through all DittoPatterns in the codeTable to update their optimal codelength

        for (auto it_blockList : *ct)
#ifdef MISS
            it_blockList->updateCodelength(sum, missSum, g_codeTable->getMathUtil(), par->nrOfAttributes);
#else
            it_blockList->updateCodelength(sum);
#endif
    }
}


Cover::~Cover() = default;
