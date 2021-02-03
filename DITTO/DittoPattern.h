#ifndef PATTERN_H
#define PATTERN_H

#include "stdafx.h"
#include "Window.h"
#include "Multi_event.h"
#include "mathutil.h"

using namespace std;

class DittoSequence;

class DittoPattern {

public:
    DittoPattern(int l, eventSet **eventSets, DittoSequence *s);

    DittoPattern(int l, eventSet **eventSets, DittoSequence *s, DittoPattern *x, DittoPattern *y);

    ~DittoPattern();

    void init();

    string print() const { return print(true); }

    string print(bool consoleOutput) const;

    string printFPwindows() const;

    int getAIDrank() const { return AID_rank; }

    double getSTsize() const { return szST; }

    int getSupport() const { return support; }

    list<Window *> *getMinWindows(DittoSequence *seq, bool otherData) {
        if (!otherData) return minWindows;
        else {
            if (altMinWindows != nullptr)return altMinWindows;
            altMinWindows = buildMinWindows(seq);
        }
        return altMinWindows;
    }

    bool getUsageDecreased() const { return usageDecreased; }

    int getUsage() const { return usage; }

    int getEstimatedUsage() { if (g_x == nullptr) return usage; else return min(g_x->getUsage(), g_y->getUsage()); }

    int getUsageGap() const { return usageGap; }

    int getUsageFill() const { return usageFill; }

#ifdef MISS
    int getUsageMiss() const { return usageMiss; }
#endif

    double getCodelength() const { return codelength; }

    double getCodelengthGap() const { return codelengthGap; }

    double getCodelengthFill() const { return codelengthFill; }

    double getEstimatedGain() const { return estimatedGain; }

    DittoPattern *getX() { return g_x; }

    DittoPattern *getY() { return g_y; }

    set<int> *getTotalAIDs() { return &totalAIDs; }

    int getHeightAtPos(int pos) const { return eventSets[pos]->size(); }

    int getLength() const { return length; }

    int getSize() const { return size; }

    eventSet *getSymbols(int timestep) const { return eventSets[timestep]; }

    bool overlap(Window *w, Window *nxt) const;

    bool overlap(eventSet *eventsA, eventSet *eventsB) const;
#ifdef MISS
    void updateUsages(int usgGap, int usgMiss) {
        usage++;
        usageGap += usgGap;
        usageFill += length - 1;
        usageMiss += usgMiss;
    }
#else
    void updateUsages(int usgGap) {
        usage++;
        usageGap += usgGap;
        usageFill += length - 1;
    }
#endif

    double computeEstimatedGain(int usgX, int usgY, int usgZ, int usgS) const;

    void setEstimatedGain(double gain) { estimatedGain = gain; }

    void loadWindowsAndSupport(DittoPattern *p) {
        support = p->getSupport();
        minWindows = p->getMinWindows(g_seq, false);
    }                    //only used for g_blackList/whitelist

    void set_info(const string &s) { g_info += s; }

    void setMinWindows(DittoSequence *s) { minWindows = buildMinWindows(s); }

    void resetUsage();
#ifdef MISS
    void updateCodelength(double sum, double missSum, mathutil* mu, int nrOfAttributes);
#else
    void updateCodelength(double sum); //sum = sum of all usages + laplace
#endif
    void rollback();


private:
    list<Window *> *buildMinWindows(DittoSequence *s);

    DittoSequence *g_seq;
    set<int> totalAIDs;        //over all time steps the set of attributes contained in this pattern
    int length;                //the number multi_events it contains
    int size;                //total nr of events in the pattern
    int AID_rank;            //rank based on the attributes it specifies values for, i.e. for every attribute we have a 1-bit when the pattern has a value for it, and a 0-bit otherwise
    double szST;            //size of the pattern encoded with base code lengths
#ifdef MISS
    double codelength, codelengthGap, codelengthFill, codelengthMiss;
#else
    double codelength, codelengthGap, codelengthFill;        //given a covering: the codeLength of the pattern, a gap in this pattern, and a fill in this pattern
#endif
    double estimatedGain;    //expected gain in L(CT,D) when adding this pattern to the code table
#ifdef MISS
    int usage, usageGap, usageFill, usageMiss;
#else
    int usage, usageGap, usageFill;
#endif
    int support;            //max nr of disjoint minimal windows

    string g_info;            //breakdown info for generated patterns

    eventSet **eventSets;        //a pointer to a set of events for each timestep

    list<Window *> *minWindows;        //a list of all minimal windows for this pattern
    list<Window *> *altMinWindows;    //alternative list of all minimal windows for this pattern in another sequence

    DittoPattern *g_x, *g_y;            //the two patterns from which this pattern is build. NOTE: x < y

    //for rollback
    double r_codelength, r_codelengthGap, r_codelengthFill;
    int r_usage, r_usageGap, r_usageFill;
    bool usageDecreased;

};


//Lowest ordered pattern at start of the list
//RETURN true if lhs < rhs 
inline bool operator<(const DittoPattern &lhs, const DittoPattern &rhs) {
    //descending on cardinality ||X||
    if (lhs.getSize() < rhs.getSize())
        return false;
    if (lhs.getSize() > rhs.getSize())
        return true;

    //descending on L(X|ST)
    if (lhs.getSTsize() < rhs.getSTsize())
        return false;
    if (lhs.getSTsize() > rhs.getSTsize())
        return true;

/*	//ascending on AID-rank
	if (lhs.getAIDrank() > rhs.getAIDrank())
		return false;
	if (lhs.getAIDrank() < rhs.getAIDrank())
		return true;
*/
    //ascending lexicographically
    //loop over all events from top to bottom and left to right
    for (int ts = 0; ts < max(lhs.getLength(), rhs.getLength()); ++ts) {
        //ascending on length
        if (ts > lhs.getLength() -
                 1)                //up to here everything equal, but rhs is longer than lhs, thus lhs has higher order
            return true;
        if (ts > rhs.getLength() - 1)
            return false;

        eventSet *avts = lhs.getSymbols(ts), *bvts = rhs.getSymbols(ts);
        auto ita = avts->begin(), enda = avts->end(), itb = bvts->begin(), endb = bvts->end();
        while (ita != enda && itb != endb) {
            Event *eva = (*ita);
            Event *evb = (*itb);

            if (*eva < *evb)        //NOTE: we compare events NOT integers
                return true;
            if (*evb < *eva)
                return false;

            ++ita;
            ++itb;
            if (ita == enda &&
                itb != endb)        //lhs is smaller on this time step but all its events are similar to rhs
                return true;
            if (itb == endb && ita != enda)
                return false;
        }
    }

    return false;
}

inline bool operator>(const DittoPattern &lhs, const DittoPattern &rhs) { return rhs < lhs; }

inline bool operator<=(const DittoPattern &lhs, const DittoPattern &rhs) { return !(lhs > rhs); }

inline bool operator>=(const DittoPattern &lhs, const DittoPattern &rhs) { return !(lhs < rhs); }

inline bool operator==(const DittoPattern &lhs, const DittoPattern &rhs) { return (!(lhs < rhs) && !(rhs < lhs)); }

inline bool operator!=(const DittoPattern &lhs, const DittoPattern &rhs) { return !(lhs == rhs); }


struct DittoPatternPtrComp    //for code table
{
    bool operator()(const DittoPattern *lhs, const DittoPattern *rhs) const {
        return *lhs < *rhs;
    }
};


//COVER ORDER: descending on cardinality, descending on support, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct CodeTableDittoPatternPtrComp    //for code table elements
{
    bool operator()(const DittoPattern *lhs, const DittoPattern *rhs) const {
        //descending on cardinality
        if (lhs->getSize() < rhs->getSize())
            return false;
        if (lhs->getSize() > rhs->getSize())
            return true;

        //descending on support
        if (lhs->getSupport() < rhs->getSupport())
            return false;
        if (lhs->getSupport() > rhs->getSupport())
            return true;

        return *lhs < *rhs;
    }
};

//CANDIDATE ORDER: descending on gain, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct CandidateDittoPatternPtrComp    //NOTE: top candidate is positioned at begin of the set
{
    bool operator()(const DittoPattern *lhs, const DittoPattern *rhs) const {
        //descending on estimated gain
        if (lhs->getEstimatedGain() < rhs->getEstimatedGain())
            return false;
        if (lhs->getEstimatedGain() > rhs->getEstimatedGain())
            return true;

        //descending on support
        if (lhs->getSupport() < rhs->getSupport())
            return false;
        if (lhs->getSupport() > rhs->getSupport())
            return true;

        return *lhs < *rhs;
    }
};

//USAGE ORDER: descending on usage, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct UsageDittoPatternPtrComp {
    bool operator()(const DittoPattern *lhs, const DittoPattern *rhs) const {
        //descending on usage
        if (lhs->getUsage() < rhs->getUsage())
            return false;
        if (lhs->getUsage() > rhs->getUsage())
            return true;

        //descending on support
        if (lhs->getSupport() < rhs->getSupport())
            return false;
        if (lhs->getSupport() > rhs->getSupport())
            return true;

        return *lhs < *rhs;
    }
};

//PRUNE ORDER: ascending on usage, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct PruneDittoPatternPtrComp    //for prune candidates -> prune-candidate with smallest usage is considered first and is positioned at the end of the set
{
    bool operator()(const DittoPattern *lhs, const DittoPattern *rhs) const {
        //ascending on usage
        if (lhs->getUsage() < rhs->getUsage())
            return true;
        if (lhs->getUsage() > rhs->getUsage())
            return false;

        //descending on support
        if (lhs->getSupport() < rhs->getSupport())
            return false;
        if (lhs->getSupport() > rhs->getSupport())
            return true;

        return *lhs < *rhs;
    }
};

typedef std::set<DittoPattern *, DittoPatternPtrComp> patternSet;                    //set of patterns

typedef std::set<DittoPattern *, CodeTableDittoPatternPtrComp> codeTableSet;        //set of patterns

typedef std::set<DittoPattern *, CandidateDittoPatternPtrComp> candpatternSet;        //set of patterns

typedef std::set<DittoPattern *, UsageDittoPatternPtrComp> usagepatternSet;        //set of patterns

typedef std::set<DittoPattern *, PruneDittoPatternPtrComp> prunepatternSet;        //set of patterns


struct usgSz {
    usgSz(int usg, double sz) : sz(sz), usg(usg) {}

    //public String toString() { cout << "Size: " << sz << "  Total usage: " << usg << endl; }

    double sz;
    int usg;
};

inline std::ostream &operator<<(std::ostream &Str, usgSz const &v) {
    Str << "Size: " << v.sz << "  Total usage: " << v.usg << endl;
    return Str;
}

struct attrSym {
    attrSym(int aid, int sym) : aid(aid), sym(sym) {}

    string print() const {
        std::stringstream ss;
        ss << "[" << aid << ", " << sym << "] ";
        return ss.str();
    }

    int aid, sym;
};

typedef set<attrSym *> attrSymSet;

struct dummy {
    dummy(int size, int length, attrSymSet **events, int support, float gapChance) : size(size), length(length),
                                                                                     events(events), support(support),
                                                                                     gapChance(gapChance) {
    }

    bool containsAttrSym(int ts, int aid, int sym) const {
        for (auto it : *events[ts])
            if (it->aid == aid && it->sym == sym)
                return true;
        return false;
    }

    string toString() const {
        stringstream ss;
        ss << "sz: " << size << "\tlen: " << length;
        for (int l = 0; l < length; ++l) {
            ss << "{";
            auto it = events[l]->begin(), end = events[l]->end();
            while (it != end) {
                ss << (*it)->aid << "." << (*it)->sym;
                ++it;
                if (it != end) ss << ",";
            }
            ss << "}";
        }
        ss << "\tsup: " << support << "\tgapChance: " << gapChance;
        return ss.str();
    }

    int size, length, support;
    float gapChance;
    attrSymSet **events;    //per timestep a pointer to a set of attrSym
};

struct Parameters {
    string header() {
        return "date; runtime; ST size; CT size; Perc. of orignal; |CT|; |non_singletons|; #exact; #subset; #union_subset; #unrelated; minsup; #multi_events; #attributes; alphabetSize; alphabetSizePerAttribute; #patterns; #patternfile; cntMatPat; cntCovers; cntAcc; cntRej; cntAccVar; cntRejVar; cntInfreqMaterialized; cntInfreq; input; output; gapvariants; blacklist; whitelist; pruneEstGain; prune_tree; input_type\n";
    }

    string toString() {
        stringstream ss;
        string date = ctime(&start);
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

        ss << date << "; " << eind - start << "; " << STsize << "; " << CTsize << "; " << perc << "; " << lengthCT
           << "; " << nr_non_singletons << "(";

        for (auto &it : *nr_non_singletons_per_size)
            ss << it.sz << "-" << it.usg << " ";
        ss << "); ";
        if (!dummy_file.empty())
            ss << cntExact << "; " << cntSubset << "; " << cntUnion_subset << "; " << cntUnrelated << "; ";
        else
            ss << "-; -; -; -;";
        ss << minsup << "; ";
        ss << nrMulti_events << "; " << nrOfAttributes << "; " << alphabetSize << "; ";
        for (int a = 0; a < nrOfAttributes; ++a)
            ss << " " << alphabetSizes[a];
        ss << "; " << nrOfDittoPatterns << "; " << dummy_file << "; ";
        ss << cntMatPat << "; " << cntCovers << "; " << cntAcc << "; " << cntRej << "; " << cntAccVar << "; "
           << cntRejVar << "; " << cntInfreqMaterialized << "; " << cntInfreq << "; " << inputFilename << "; "
           << outputFilename << "; " << gapvariants << "; " << blacklist << "; " << whitelist << "; " << pruneEstGain
           << "; " << prune_tree << "; " << input_type << "\n";
        return ss.str();
    }

    string print() {
        stringstream ss;
        string date = ctime(&start);
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

        ss << "date: " << date << "\nruntime: " << eind - start << "\ninp: " << inputFilename << "\noutp: "
           << outputFilename << endl;
        ss << "STsize: " << STsize << "\nCTsize: " << CTsize << "\n% of orig: " << perc << "\n|CT|: " << lengthCT
           << "\n |non_singletons|: " << nr_non_singletons << "(";

        for (auto it: *nr_non_singletons_per_size)
            ss << it.sz << "-" << it.usg << " ";

        ss << ")\n" << "Breakdown of found patterns, hidden: " << nrOfDittoPatterns << " total found: "
           << nr_non_singletons << " Exact: " << cntExact << " Subset: " << cntSubset << " Union_subset: "
           << cntUnion_subset << " unrelated: " << cntUnrelated << endl;
        ss << "cntMatPat: " << cntMatPat << "  cntCovers: " << cntCovers << "  cntAcc: " << cntAcc << "  cntRej: "
           << cntRej << "  cntAccVar: " << cntAccVar << "  cntRejVar: " << cntRejVar << "  cntInfreqMat: "
           << cntInfreqMaterialized << "  cntInfreq: " << cntInfreq << "\n";
        return ss.str();
    }

    bool release,                //no system("pause");
    runtimes,                //print runtime info to file
    gapvariants,            //if a pattern is accepted we recursively try this candidate with all its gap events
    blacklist,                //contains all materialized patterns with sup=0
    whitelist,                //contains all materialized patterns
    pruneEstGain,            //do not consider candidates with estimated gain <= 0
    prune_tree,                //quickly discard new candidates based on minimum support
    FPwindows,                //to print the windows of the top of the code table to file to plot the occurences with python
    fillDittoPatterns,            //to turn fillDittoPattern on or off
    *fillDittoPattern;            //for each attribute whether fillPatters are used. To make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)

    int minsup,                    //a minimum support threshold for candidate patterns
    input_type,                //CATEGORICAL or ITEMSET
    nrMulti_events,
            nrOfAttributes,
            tempAlphabetSize,        //for generated data we only specify the alphabetSize for a single attribute (similar for all attributes)
    alphabetSize,            //total over all attributes
    *alphabetSizes,            //per attribute
    nrOfDittoPatterns;            //synthetic patterns

    DittoSequence *seq;                    //the data


    // RESULTS
    int lengthCT, nr_non_singletons, cntExact, cntSubset, cntUnion_subset, cntUnrelated;
    list<usgSz> *nr_non_singletons_per_size;        //'usg' is the number of times a non-singleton of 'sz' occurs in the CT
    float STsize, CTsize, perc;

    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    bool debug, debug2,            //print debug info
    pruneCheck;                //reports when a pattern is pruned that was not part of the recently added pattern

    int cntMatPat, cntAcc, cntRej, cntAccVar, cntRejVar, cntInfreqMaterialized, cntInfreq, cntCovers; //DEBUG 22-10

    string outputFilename,
            inputFilename,
            dummy_file;


    time_t start, eind;

    dummy **dummies;                //a list of the inserted dummy-patterns

};


#endif
