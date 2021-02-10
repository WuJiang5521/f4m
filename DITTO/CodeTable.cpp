#include "stdafx.h"
#include "CodeTable.h"

using namespace std;

double logb2(double d) {
    if (d == 0)
        return 0;
    else
        return lg2(d);
}

CodeTable::CodeTable(DittoSequence *s) : g_sequence(s) {
    g_codeTable = new codeTableSet;
    length_CT = 0;
    szCT_C = 0;

    par = g_sequence->getParameters();
    mu = g_sequence->getMu();
    g_STcodelengths = g_sequence->get_STcodelengths();
}

// L(D | CT)
double CodeTable::compute_szD_CT(DittoSequence *sequence) {
    double size = 0;

    size += mu->intcost(par->nrOfAttributes);                                            //TERM: L_N(|A|)
    size += mu->intcost(sequence->get_nrSequences());                                    //TERM: L_N(s(D))

    int *seqSizes = sequence->get_sequence_sizes();
    for (int s = 0;
         s < sequence->get_nrSequences(); ++s)                                //TERM: sum_{j in s(D)} L_N(|S_j|)
        size += mu->intcost(seqSizes[s]);

    //sum over all DittoPatterns in the codeTable: the multiplication of their usage by their codelength
    for (auto it_ct : *g_codeTable) {
        //skip fill-patterns
        if (par->fillDittoPatterns && it_ct->getSize() == 1 &&
            par->fillDittoPattern[(*it_ct->getSymbols(0)->begin())->attribute] &&
            (*it_ct->getSymbols(0)->begin())->symbol ==
            par->alphabetSizes[(*it_ct->getSymbols(0)->begin())->attribute] - 1)
            continue;

        if (it_ct->getUsage() > 0) {
            size += it_ct->getUsage() * it_ct->getCodelength();                    //TERM: L(C_p | CT)

            if (it_ct->getLength() > 1)    // |X| > 1 -> possible gaps
            {
                size += it_ct->getUsageGap() *
                        it_ct->getCodelengthGap();            //TERM: L(C_g | CT)		-> gap codes
                size += it_ct->getUsageFill() *
                        it_ct->getCodelengthFill();        //TERM: L(C_g | CT)		-> no_gap codes
#ifdef MISS
                if (it_ct->getUsageMiss() > 0) {
                    size += it_ct->getUsageMiss() *
                            it_ct->getCodelengthMiss();
                }
#endif
            }
        }
    }

    if (sequence->getInputType() == ITEMSET)
        size += par->nrMulti_events * logb2(par->alphabetSize);                            //TERM: sum_|D| log(|Omega|)

    return size;
}


// L(CT | C)
double CodeTable::compute_szCT_C(DittoSequence *sequence) {
    double size = 0;
    int *alphabetSizes = par->alphabetSizes;
    int nrAttr = par->nrOfAttributes;

    if (sequence->getInputType() == CATEGORICAL) {
        for (int aid = 0; aid < nrAttr; ++aid) {
            int alph = alphabetSizes[aid];
            if (par->fillDittoPatterns && par->fillDittoPattern[aid])
                alph -= 1;
            size += mu->intcost(
                    alph);                                                    //TERM: L_N(|Omega_aid|)					-> #singletons
            size += mu->lg_choose(par->nrMulti_events,
                                  alph);                            //TERM: log( ||D^aid|| OVER |Omega_aid| )	-> singleton supports
        }
    } else //ITEMSET
        size += mu->lg_choose(sequence->get_nrEvents(),
                              par->nrOfAttributes);            //TERM: log(||D|| OVER |A|)					-> support for all attributes; divide all events over all singletons. NOTE: singletons can have zero support -> thus NOT log(||D||-1 OVER |A|-1)

    int non_singletons = length_CT;
    for (int aid = 0; aid < nrAttr; ++aid)
        non_singletons -= alphabetSizes[aid];
    int non_singl_usg = 0;
    for (auto p : *g_codeTable) {  //TERM: L(X in CT) for all non-singleton patters
        if (p->getSize() == 1)
            continue;                                                                    //skip singletons

        if (p->getUsage() == 0)
            continue;

        non_singl_usg += p->getUsage();
        size += mu->intcost(
                p->getLength());                                            //TERM: L_N( |X| )							-> length of the pattern

        for (int ts = 0; ts < p->getLength(); ++ts)
            size += logb2(
                    nrAttr);                                                        //TERM: log( |A| )							-> height of the pattern at ts

        size += mu->intcost(p->getUsageGap() +
                            1);                                        //TERM: L_N( gaps(X)+1 )					-> #gaps in X
        size += p->getSTsize();                                                            //TERM: SUM_{x in X} L( code_p(x|ST) )		-> length of X in left column of CT
    }

    size += mu->intcost(non_singletons +
                        1);                                            //TERM: L_N( |P|+1 )						-> #non-singletons

    size += mu->intcost(non_singl_usg +
                        1);                                                //TERM: L_N( usage(P)+1 )					-> total usage of non-singletons

    if (non_singl_usg != 0) {
        if (non_singl_usg <
            non_singletons)                                                //one non-singletons with usage=0, rest with usage=1
            size += logb2(
                    non_singletons);                                                //indicate the one with usage=0
        else
            size += mu->lg_choose(non_singl_usg,
                                  non_singletons);                        //TERM: log( usage(P) OVER |P|)				-> the usage per non-singleton pattern
    }

    return size;
}


/* Return:		L(D, CT) = L(D|CT) + L(CT)
   Parameters:	dataStream: either where this codeTable is build on or another arbitrary stream
*/
double CodeTable::compute_sz(DittoSequence *sequence) {
    double size = 0;
    size += compute_szD_CT(sequence);                //L(D | CT)

    double temp = compute_szCT_C(sequence);
    if (szCT_C ==
        0)                                    //when the final CT has been computed we can reuse this size because it does not change when encoding other data
        size += temp;            //L(CT | C)
    else
        size += szCT_C;

    return size;
}

// We want to have the highest ordered pattern at the start of the list
//Returns true when the pattern was added, i.e. not yet present
bool CodeTable::insertDittoPattern(DittoPattern *p) {
    std::pair<codeTableSet::iterator, bool> ret;
    ret = g_codeTable->insert(p);
    if (ret.second)            //not yet present
        length_CT++;

    return ret.second;
}

void CodeTable::deleteDittoPattern(DittoPattern *p) {
    if (p->getSize() == 1) {
        cout << "Trying to delete a singleton pattern!\n";
        return;
    }

    auto fnd = g_codeTable->find(p), end = g_codeTable->end();
    if (fnd != end) {
        g_codeTable->erase(fnd);
        length_CT--;
    }
}


//delete all DittoPatterns for which the usage dropped to zero in the final code table
void CodeTable::deleteUnusedDittoPatterns() {
    auto it_ct = g_codeTable->begin(), it_end = g_codeTable->end();
    while (it_ct != it_end) {
        if ((*it_ct)->getSize() == 1) {
            ++it_ct;
            continue;                                    //EXCEPT singletons!
        }

        if ((*it_ct)->getUsage() == 0) {
            it_ct = g_codeTable->erase(it_ct);
            length_CT--;
            it_end = g_codeTable->end();
            if (it_ct == it_end)
                break;
        } else
            ++it_ct;                                    //only if no block is erased we have to shift the iterator
    }
}

//rollback all changed patterns
void CodeTable::rollback() {
    auto it_ct = g_codeTable->begin(), it_end = g_codeTable->end();
    while (it_ct != it_end) {
        (*it_ct)->rollback();
        ++it_ct;
    }
}


string CodeTable::printcandidateSet(bool consoleOutput, candpatternSet *plist, const string &title) const {
    stringstream result;
    result << endl << title << endl;
    candpatternSet::iterator it_patternSet, it_end = plist->end();
    for (it_patternSet = plist->begin(); it_patternSet != it_end; ++it_patternSet) {
        result << "\t\t";
        result << (*it_patternSet)->print(false);
    }
    result << endl;

    if (consoleOutput)
        cout << result.str();
    return result.str();
}

string CodeTable::printpatternSet(bool consoleOutput, codeTableSet *plist, const string &title) const {

    stringstream result;
    result << endl << title << endl;
    codeTableSet::iterator it_patternSet, it_end = plist->end();
    for (it_patternSet = plist->begin(); it_patternSet != it_end; ++it_patternSet) {
        result << "\t\t";
        result << (*it_patternSet)->print(false);
    }
    result << endl;

    if (consoleOutput)
        cout << result.str();
    return result.str();
}


string CodeTable::printCT(bool consoleOutput) const {
    return printpatternSet(consoleOutput, g_codeTable, "Code Table");
}

list<usgSz> *CodeTable::get_nr_non_singletons_per_size() {
    auto *result = new list<usgSz>;

    int cnt = 0;
    int szOld = (*g_codeTable->begin())->getSize();
    for (auto it : *g_codeTable) {
        int szNew = it->getSize();
        if (szOld == szNew)
            cnt++;
        else {
            usgSz us(cnt, szOld);
            result->push_back(us);
            cnt = 1;
            szOld = szNew;
        }
    }
    return result;
}

CodeTable::~CodeTable() {
    //g_codeTable->clear();
    delete g_codeTable;
}

