#include "stdafx.h"
#include "DittoPattern.h"
#include "DittoSequence.h"

double log2(double d) {
    if (d == 0)
        return 0;
    else
        return lg2(d);
}

/*
Parameters:
	int length:				# of time steps the pattern is long
	eventSet **eventSets:	for each time step a pointer to set of events	
*/
DittoPattern::DittoPattern(int length, eventSet **eventSets, DittoSequence *s) : length(length), eventSets(eventSets),
                                                                                 g_seq(s) {
    g_x = nullptr;
    g_y = nullptr;
    init();
    setMinWindows(s);
}

DittoPattern::DittoPattern(int length, eventSet **eventSets, DittoSequence *s, DittoPattern *x, DittoPattern *y)
        : length(length), eventSets(eventSets), g_seq(s) {
    if (x != nullptr && y != nullptr) {
        //Order x and y such that x is always the one ordered lower
        if (*x > *y) {
            g_x = y;
            g_y = x;
        } else {
            g_x = x;
            g_y = y;
        }
    }
    init();
}


void DittoPattern::init() {
    g_info = "";
    minWindows = nullptr;
    support = 0;
    altMinWindows = nullptr;

    codelength = DBL_MAX;
    codelengthGap = DBL_MAX;
    codelengthFill = DBL_MAX;
    r_codelength = DBL_MAX;
    r_codelengthGap = DBL_MAX;
    r_codelengthFill = DBL_MAX;
#ifdef MISS
    codelengthMiss = DBL_MAX;
    r_codelengthMiss = DBL_MAX;
#endif
    estimatedGain = 0;

    usage = 0;
    usageGap = 0;
    usageFill = 0;
#ifdef MISS
    usageMiss = 0;
#endif
    r_usage = 0;
    r_usageGap = 0;
    r_usageFill = 0;
#ifdef MISS
    r_usageMiss = 0;
#endif

    usageDecreased = false;

    size = 0;                            //nr of events in the pattern
    szST = 0;                            //encoded size given ST
    totalAIDs = set<int>();                //compute the AIDs over all time steps
    double **baseCodeLengths = g_seq->get_STcodelengths();
    for (int l = 0; l < length; ++l) {
        for (auto e : *eventSets[l]) {
            totalAIDs.insert(e->attribute);
            size += 1;                                            //the number of events in the pattern
            szST += baseCodeLengths[e->attribute][e->symbol];    //the ST codelength for this event
        }
    }

    //set AID_rank
    AID_rank = 0;
    for (int totalAID : totalAIDs) {
        AID_rank += g_seq->getMu()->power2(totalAID);        //aid=0 adds 1 to AID_rank, aid=1 adds 2 to AID_rank etc..
    }
}

string DittoPattern::print(bool consoleOutput) const {
    stringstream result;
    result << "--- sz: " << size << "  len: " << length << "  h: " << totalAIDs.size() << "\t";
    for (int l = 0; l < length; ++l) {
        result << "{";
        auto it = eventSets[l]->begin(), end = eventSets[l]->end();
        while (it != end) {
            //in ITEMSET case the attribute indicates the symbol, because each attribute is either present or not, but they are switched at init
            result << (*it)->attribute << "." << (*it)->symbol;
            if (++it != end) result << ",";
        }
        result << "}";
    }

    if (!g_seq->getParameters()->dummy_file.empty())
        result << "\t" << g_info;

    result << "\tSTlen: " << szST;
    result << "\tsup: " << support;
    result << "\tusg: " << usage;
    if (size != 1) result << "\tusgGap: " << usageGap;
//	result << "\tusgFill: " << usageFill;
    result << "\tcl: " << codelength;
    if (size != 1) result << "\tgain: " /*<< std::setprecision(15)*/ << estimatedGain;
    //result << "\tusg gap: " << usageGap;
//	result << "\tadr: " << this;


    //print minimal windows
//        if (support != 0) {
//            result << "\tminWin: ";
//            list<Window *>::iterator it = minWindows->begin(), end = minWindows->end();
//            while (it != end) {
//                result << "(" << (*it)->first->id << "," << (*it)->last->id << ") ";
//                ++it;
//            }
//        }

    //print active windows
    if (usage != 0) {
        result << "\tactiveWindows: ";
        for (auto w : *minWindows)  //loop over its minimum windows
            if (w->active)
                result << "(" << w->first->id << "," << w->last->id << ") ";
    }

    result << endl;

    if (consoleOutput)
        cout << result.str();
    return result.str();
}

string DittoPattern::printFPwindows() const {
    stringstream result;
    result << minWindows->size() /*support*/ << " " << usage << " " << usageGap << " " << length << " ";
    for (int l = 0; l < length; ++l) {
        result << eventSets[l]->size() << " ";
        for (auto it : *eventSets[l])
            result << it->attribute << " " << it->symbol << " ";
    }
    result << endl;

    for (auto w : *minWindows) { //loop over its minimum windows
        result << w->first->id << " " << w->last->id - w->first->id + 1 << " " << w->active << " ";
        for (int it : *w->get_gaps())
            result << it << " ";
    }
    result << endl;
    return result.str();
}

double DittoPattern::computeEstimatedGain(int usgX, int usgY, int usgZ, int usgS) const {
    double gain = 0;

    bool similar = false;
    if (*g_x == *g_y)
        similar = true;

    int usgX2;
    if (similar) {
        if (usgZ % 2 == 1)
            usgX2 = 1;                                    //X and Y are similar
        else
            usgX2 = 0;
        usgZ /= 2;
    } else
        usgX2 = usgX - usgZ;                        //usage X after adding Z

    int usgY2 = usgY - usgZ;                        //usage Y after adding Z
    int usgS2 = usgS - usgZ;                        //total usage after adding Z

    int nrAttr = g_seq->getParameters()->nrOfAttributes;
    mathutil *mu = g_seq->getMu();

    //DELTA L(CT+Z | C)
    gain -= mu->intcost(length);                                // length of the pattern = L_N( |X| )
    for (int ts = 0; ts < length; ++ts)
        gain -= mu->intcost(nrAttr);                            // height of the pattern at ts = log( |A| )

    gain -= szST;                                                // SUM_{x in X} L( code_p(x|ST) )

    //DELTA L(D | CT+Z)
    gain += usgS * log2(usgS);
    gain -= usgS2 * log2(usgS2);
    gain += usgZ * log2(usgZ);
    gain -= (usgX * log2(usgX) - usgX2 * log2(usgX2));
    if (!similar)
        gain -= (usgY * log2(usgY) - usgY2 * log2(usgY2));

    return gain;
}


//NOTE: usage must be set before this method, i.e. cover must be run
#ifdef MISS
void DittoPattern::updateCodelength(double sum, double missSum, mathutil* mu, int nrOfAttributes)
#else
void DittoPattern::updateCodelength(double sum)    //sum includes laplace for every pattern
#endif
{
    codelength = -log2((usage + laplace) / sum);

#ifdef MISS
    codelengthGap = -log2((usageGap + laplace) / (usageFill + usageGap + usageMiss + 2 * laplace));

    codelengthFill = -log2((usageFill + laplace) / (usageFill + usageGap + usageMiss + 2 * laplace));
    codelengthMiss = -log2((usageMiss + laplace) / (usageFill + usageGap + usageMiss)) + mu->intcost(nrOfAttributes);
#else

    codelengthGap = -log2((usageGap + laplace) / (usageFill + usageGap + 2 * laplace));

    codelengthFill = -log2((usageFill + laplace) / (usageFill + usageGap + 2 * laplace));
#endif

    //Check if the total usage has decreased
    if (r_usage > usage)
        usageDecreased = true;
    else
        usageDecreased = false;
}


/*loop over all occurences for the last Multi_event in this pattern and do the following:
	-step back until we have a window that contains all Multi_events of the pattern -> not always minimal
	-from first Multi_event in the pattern step forward to find minimal window within the recently found window
	-note: when a Multi_event can be located at multiple timesteps in the minimal window, in this approach we always choose the first possibility in time. E.g. we choose the first b when covering the minimal window abbc with pattern abc.
*/
list<Window *> *DittoPattern::buildMinWindows(DittoSequence *s) //NOTE: 's' might be different from 'g_seq'
{
    support = 0;
    auto *result = new list<Window *>();
    const auto **occPerTimestep = new const list<Multi_event *> *[length];
    int *supPerTimestep = new int[length];
    for (int l = 0; l < length; ++l) {
        occPerTimestep[l] = s->findOccurrences(eventSets[l]);
        supPerTimestep[l] = occPerTimestep[l]->size();
        if (supPerTimestep[l] == 0)
            return result;    //There can be no minimal windows for this pattern
    }

    if (length > 1) {
        const Multi_event **mev_positions;
        const Multi_event *front, *back;
        auto it_last = occPerTimestep[length - 1]->rbegin(),
                end_last = occPerTimestep[length - 1]->rend();
        while (it_last != end_last) //loop over all occurences of the last Multi_event in the pattern
        {
            mev_positions = new const Multi_event *[length];
            front = nullptr;
            back = *it_last;
            int id = back->id;

            bool stop = false;
            //find a window that ends at back->id
            mev_positions[length - 1] = back;
            for (int pos = length - 2; pos >= 0; --pos)    //skip the last
            {
                auto it = occPerTimestep[pos]->rbegin(), end = occPerTimestep[pos]->rend();    //reverse iterator
                while ((*it)->id >= id) {
                    ++it;
                    if (it == end) {
                        stop = true;
                        break;
                    }
                }

                if (!stop && (back->id - (*it)->id) + 1 >
                             2 * length - 1)  // max gapsize is patternlength-1 -> total length = 2*patternlength-1
                    stop = true;

                if (stop)
                    break;

                id = (*it)->id;
                mev_positions[pos] = *it;
                if (pos == 0)
                    front = *it;
            }

            //make a minimal window out of the found window
            if (!stop && front != nullptr)    //if a window was found
            {
                int cur_id = front->id;
                for (int pos = 1; pos < length; ++pos)    //first Multi_event is already in right place
                {
                    auto it = occPerTimestep[pos]->begin(), end = occPerTimestep[pos]->end();    //iterator
                    while ((*it)->id <= cur_id) ++it;
                    mev_positions[pos] = *it;
                    cur_id = (*it)->id;
                }
            }

            if (!stop && front != nullptr && front->seqid == back->seqid) {
                auto *w = new Window(mev_positions, this);
                bool present = false;

                if (!result->empty()) {
                    if (w->equal(result->back()))
                        present = true;
                    else
                        w->next = result->back();
                }
                if (present)
                    delete w;
                else
                    result->push_back(w);            //the windows are added with the last starting window first
            } else
                delete[]mev_positions;

            ++it_last;
        }

        //set prev/nextDisjoint windows, NOTE: minWindows are ordered with the first starting window at the end of the list
        if (result->size() > 1) {
            Window *w, *nxt = result->back()->next;
            auto it = result->rbegin(), end = result->rend();
            while (it != end) {
                w = *it;

                //for w its next disjoint windows can not be before nxt, thus we can continue with the nxt variable as long as it comes after w
                if (nxt->first->id <= w->first->id)
                    nxt = w->next;

                //check if there are events that overlap
                while (overlap(w, nxt))
                    nxt = nxt->next;

                if (nxt == nullptr) //all further windows will also have no nextDisjoint
                    break;

                w->nextDisjoint = nxt;
                nxt->prevDisjoint = w;
                ++it;
            }
        }

        //set support (max nr of disjoint minimal windows): we greedily add the minimal window that starts/ends first (because all windows are minimal it is also the window that ends first)
        if (!result->empty()) {
            //NOTE: minWindows are ordered with the first starting window at the back of the list
            Window *greedy = result->back();
            while (greedy != nullptr) {
                ++support;
                greedy = greedy->nextDisjoint;
            }
        }
    } else {   //length == 1, thus every occurence is a minWindow
        for (auto it : *occPerTimestep[0]) {
            ++support;
            const auto **mev_positions = new const Multi_event *[1];
            mev_positions[0] = it;
            auto *w = new Window(mev_positions, this);
            if (!result->empty())
                result->back()->next = w;
            result->push_back(w);
        }
    }

    return result;
}


//Return true when the two event sets overlap on an event
bool DittoPattern::overlap(eventSet *eventsA, eventSet *eventsB) const {
    auto endB = eventsB->end();
    for (auto it : *eventsA)
        if (eventsB->find(it) != endB)
            return true;
    return false;
}

//Return true when the two windows overlap on an event
bool DittoPattern::overlap(Window *w, Window *nxt) const {
    if (nxt == nullptr)
        return false;

    //determine offset for overlap
    int offset = nxt->first->id - w->first->id;

    //for the overlapping positions check if the two corresponding event-sets of this pattern overlap
    for (int posA = length - 1; posA >=
                                offset; --posA)    //NOTE: offset >= patternlength means no overlap of Multi_events -> is automatically skipped by this loop
    {
        if (overlap(eventSets[posA], eventSets[posA - offset]))
            return true;
    }
    return false;
}

void DittoPattern::resetUsage() {
    //backup usage
    r_usage = usage;
    r_usageGap = usageGap;
    r_usageFill = usageFill;
    usage = 0;
    usageGap = 0;
    usageFill = 0;
#ifdef MISS
    r_usageMiss = usageMiss;
    usageMiss = 0;
#endif

    r_codelength = codelength;
    r_codelengthGap = codelengthGap;
    r_codelengthFill = codelengthFill;
    //leave the codelength itself untouched!
#ifdef MISS
    r_codelengthMiss = codelengthMiss;
#endif

    //set al windows to active = false

    for (auto &minWindow : *minWindows)
        minWindow->active = false;
}

void DittoPattern::rollback() {
    usage = r_usage;
    usageGap = r_usageGap;
    usageFill = r_usageFill;
    codelength = r_codelength;
    codelengthGap = r_codelengthGap;
    codelengthFill = r_codelengthFill;
#ifdef MISS
    usageMiss = r_usageMiss;
    codelengthMiss = r_codelengthMiss;
#endif
}

DittoPattern::~DittoPattern() {
    totalAIDs.clear();

    for (int l = 0; l < length; ++l) {
        //events themselves get deleted when the multi_events are deleted, when the sequence is deleted
        delete eventSets[l];        //delete the containers, not the events
    }
    delete[]eventSets;

    if (minWindows != nullptr) {
        minWindows->clear();
        delete minWindows;
    }

    if (altMinWindows != nullptr) {
        altMinWindows->clear();
        delete altMinWindows;
    }
}

