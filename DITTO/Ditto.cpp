#include "stdafx.h"
#include "Ditto.h"

using namespace std;

int dittoEnter(int argc, char *argv[]) {
    Parameters parameters;
    parameters.release = true;

    parameters.runtimes = true;
    parameters.debug = false;
    parameters.debug2 = false;

    parameters.gapvariants = true;
    parameters.pruneCheck = false;            //mag eruit
    parameters.blacklist = false;            //mag eruit
    parameters.whitelist = false;
    parameters.pruneEstGain = true;
    parameters.prune_tree = true;
    parameters.FPwindows = false;            //to print the found patterns in the data AND to translate patterns from text-data
    parameters.fillDittoPatterns = false;        //for aligned text-data

    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    parameters.cntCovers = 0;
    parameters.cntMatPat = 0;
    parameters.cntAcc = 0;
    parameters.cntRej = 0;
    parameters.cntAccVar = 0;
    parameters.cntRejVar = 0;
    parameters.cntInfreqMaterialized = 0;
    parameters.cntInfreq = 0;

    parameters.input_type = CATEGORICAL;
    parameters.minsup = 5;
    parameters.dummy_file = "";
    parameters.inputFilename = "";

    if (parameters.release) {
        //DEFAULT values
        parameters.input_type = CATEGORICAL;
        parameters.minsup = 5;
        parameters.dummy_file = "";
        parameters.inputFilename = "";

        char opt_pattern[] = "i:t:m:p:w:f:";
        int c, pos;
        while ((c = getopt(argc, argv, opt_pattern)) != -1) {
            switch (c) {
                case 'w':
                    parameters.FPwindows = (atoi(poptarg) != 0);
                    break;
                case 'f':
                    parameters.fillDittoPatterns = (atoi(poptarg) != 0);
                    break;
                case 't':
                    parameters.input_type = atoi(poptarg);
                    break;
                case 'i':
                    parameters.inputFilename = string(poptarg);
                    pos = parameters.inputFilename.find_last_of('.');
                    if (pos != parameters.inputFilename.length() - 4 ||
                        parameters.inputFilename.substr(pos, parameters.inputFilename.length() - 1) != ".dat") {
                        cout << "ERROR: inputfile must be '.dat'-file.";
                        return 1;
                    }
                    break;
                case 'm':
                    parameters.minsup = atoi(poptarg);
                    break;
                case 'p':
                    parameters.dummy_file = string(poptarg);
                    pos = parameters.dummy_file.find_last_of('.');
                    if (pos == -1 || pos != parameters.dummy_file.length() - 4 ||
                        parameters.dummy_file.substr(pos, parameters.dummy_file.length() - 1) != ".txt") {
                        cout << "ERROR: pattern-file must be '.txt'-file.\n";
                        return 1;
                    }
                    break;
                default:
                    cout << "Usage:\n"
                         << "\t-i\t<inputfile>.dat\t\t\t\t\t(mandatory)\n"
                         << "\t-p\t<pattern-file>.txt\t\t\t\t(default='" << parameters.dummy_file << "')\n"
                         << "\t-m\tminimum support\t\t\t\t\t(default=" << parameters.minsup << ")\n"
                         << "\t-t\tinput type: 1=categorical, 2=item set\t(default=" << parameters.input_type << ")\n"
                         << "\t-w\ttrue=print all found patterns to file (also used to translate patterns from text-data)\n"
                         << "\t-f\ttrue=fill-patterns are used to aligne text-data\n";
                    return 1;
            }
        }
        if (parameters.inputFilename.empty()) {
            cout << "ERROR: <inputfile>.dat is mandatory, type -h for help.\n";
            return 1;
        }
    }

    FILE *f;
    if (!(f = fopen(parameters.inputFilename.c_str(), "r"))) {
        cout << "ERROR opening data file: " << parameters.inputFilename << "\n";
        if (!parameters.release)
            system("pause");
        return 1;
    }
    parameters.seq = new DittoSequence(f, &parameters);
    fclose(f);

    if (parameters.seq->g_error) {
        cout << "ERROR reading sequence or patterns!\n";
        if (!parameters.release)
            system("pause");
        return 1;
    }

    stringstream ss;
    ss << parameters.minsup;
    int strt = parameters.inputFilename.find_last_of("/\\") + 1, nd = parameters.inputFilename.find_last_of('.');
    string temp = parameters.inputFilename.substr(strt, nd - strt);
    parameters.outputFilename = "output_fileData_" + temp + "_minsup" + ss.str();

    if (parameters.fillDittoPatterns)    //to make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)
    {
        parameters.fillDittoPattern = new bool[parameters.nrOfAttributes];
        for (int attr = 0; attr < parameters.nrOfAttributes; ++attr)
            parameters.fillDittoPattern[attr] = true;
    }

    if (parameters.seq->getInputType() != CATEGORICAL)        //for now only possible for categorical data
        parameters.prune_tree = false;
    clock_t t0 = clock();
    auto *ditto = new Ditto(&parameters);
    cout << "DITTO time: " << (clock() - t0)*1.0/CLOCKS_PER_SEC << "s" << endl;
    delete ditto;

    if (!parameters.release)
        system("pause");
    return 0;
}
Ditto::Ditto(Parameters *par) : par(par) {
    g_outputStream << par->seq->print_sequence(false);      //DEBUG

    par->start = time(nullptr);
    par->cntExact = 0;
    par->cntSubset = 0;
    par->cntUnion_subset = 0;
    par->cntUnrelated = 0;

    //build a codeTable
    g_ct = new CodeTable(par->seq);
    g_blackList = new patternSet;
    g_whiteList = new patternSet;
    g_ct_on_usg = new usagepatternSet;        //set to combine CTxCT based on usage
#ifdef FMP
    candidateOrder = new priority_queue<pair<int, pair<DittoPattern*, DittoPattern*>>>();
#endif

    //build singletons
    int **tree_ids = par->seq->get_tree_ids();
    int nr_singletons = par->alphabetSize;
    int *alphabetSizes = par->alphabetSizes;
    for (int aid = 0; aid < par->nrOfAttributes; ++aid) {
        for (int sym = 0; sym < alphabetSizes[aid]; ++sym) {
            auto **eventSets = new eventSet *[1];
            eventSets[0] = new eventSet;
            eventSets[0]->insert(new Event(sym, aid, 0, tree_ids[aid][sym]));
            auto *p = new DittoPattern(1, eventSets, par->seq);
            g_ct->insertDittoPattern(p);
            g_ct_on_usg->insert(p);
        }
    }
#ifdef FMP
    vector<DittoPattern*> singletons;
    P_PTable::patternIDMap.clear();
    int initialPatternIDMapTop = 0;
    for (auto & p : *(g_ct->getCT())) {
        singletons.push_back(p);
        P_PTable::patternIDMap[p] = initialPatternIDMapTop++;
        P_PTable::patternIDTable.push_back(p);
    }

    P_PTable::tableSize = initialPatternIDMapTop;
    P_PTable::preTable.resize(P_PTable::tableSize);
    P_PTable::lastTable.resize(P_PTable::tableSize);
    for (int i = 0; i < P_PTable::tableSize; ++i) {
        P_PTable::preTable[i].resize(P_PTable::tableSize, 0);
        P_PTable::lastTable[i].resize(P_PTable::tableSize, 0);
    }
#endif

    //build a tree that represents all patterns of length == 1
    //each timestep in a new candidate is run through this tree to see if it still can be frequent
    g_root = new Node(nr_singletons, par->alphabetSizes, 0);

    g_cand = new candpatternSet;    //NOTE: candidates are ordered based on estimated gain

    auto it_ct_1 = g_ct_on_usg->begin(), it_ct_2 = g_ct_on_usg->begin(), begin_ct = g_ct_on_usg->begin(), end_ct = g_ct_on_usg->end();    //iterators to update candidate list

    auto *g_cover = new Cover(par->seq, g_ct, false);    //determine ST size
#ifdef FMP
    delete candidateOrder;
    candidateOrder = new priority_queue<pair<int, pair<DittoPattern*, DittoPattern*>>>();
    for (int i = 0; i < (*P_PTable::table).size(); ++i) {
        for (int j = 0; j < (*P_PTable::table)[i].size(); ++j) {
            candidateOrder->push(make_pair((*P_PTable::table)[i][j], make_pair(P_PTable::patternIDTable[i], P_PTable::patternIDTable[j])));
        }
    }
#endif

    auto *current_usgSz = new usgSz(g_cover->get_totalUsage(), g_cover->get_szSequenceAndCT());

    double STsize = current_usgSz->sz;
    g_outputStream << "\n\nST " << *current_usgSz << endl;//DEBUG
    g_outputStream << g_ct->printCT(false);//DEBUG

    while (true) {
#ifdef FMP
        if ((candidateOrder->empty() || candidateOrder->top().first < candThreshold) &&
            g_cand->empty())                //we stop when there are no more candidates to generate. NOTE it_ct_2 reaches end first
            break;
        generateCandidates(current_usgSz);
#else
        if (it_ct_2 == end_ct &&
        g_cand->empty())                //we stop when there are no more candidates to generate. NOTE it_ct_2 reaches end first
            break;
        //update candidates
        generateCandidates(&it_ct_1, &it_ct_2, &begin_ct, &end_ct, current_usgSz);
#endif
//        cout << "*****" << endl;
        if (g_cand->empty())
            continue;

        //Add top candidate
        DittoPattern *top = *g_cand->begin();
        g_cand->erase(
                top);                            //do not consider this candidate again. NOTE: it must also be erased from g_cand to not be compared to as best candidate

        if (par->pruneEstGain && top->getEstimatedGain() <= 0)
            continue;

        loadOrBuildMinWindows(top);

        //prune on minsup
        if (top->getSupport() < par->minsup)
            continue;

        if (!g_ct->insertDittoPattern(
                top))                //pattern already present. NOTE this check must be after setMinWindows, because pattern equality is also based on support
            continue;                                //already present
#ifdef FMP
        P_PTable::checkTable();
        int patternIDMapTop = 0;
        P_PTable::patternIDTable.clear();
        for (auto & p : *(g_ct->getCT())) {
            P_PTable::patternIDMap[p] = patternIDMapTop++;
            P_PTable::patternIDTable.push_back(p);
        }

        P_PTable::tableSize = patternIDMapTop;
        if (P_PTable::tableSize > P_PTable::preTable.size()) {
            P_PTable::preTable.resize(P_PTable::tableSize);
            P_PTable::lastTable.resize(P_PTable::tableSize);
            for (int i = 0; i < P_PTable::tableSize; ++i) {
                P_PTable::preTable[i].resize(P_PTable::tableSize, 0);
                P_PTable::lastTable[i].resize(P_PTable::tableSize, 0);
            }
        }
        P_PTable::clearTable(P_PTable::tableSize);
#endif
        //Cover
        clock_t t0 = clock();
        g_cover = new Cover(par->seq, g_ct, false);
        cout << "cover time: " << clock() - t0 << endl;
        double new_size = g_cover->get_szSequenceAndCT();
        int new_total_usage = g_cover->get_totalUsage();

        cout << "current_size = " << current_usgSz->sz << "  new_size = " << new_size << endl;//DEBUG
//        cout << "ct size: " << g_ct->getCT()->size() << endl;

        //Check improvement
        if (new_size < current_usgSz->sz) {
            current_usgSz = postprune(top, new_total_usage, new_size);                //post acceptance pruning
            if (par->gapvariants)                                                    //recursively try variations of top+singleton
                current_usgSz = tryVariations(top, current_usgSz);

            //rebuild g_ct_on_usg because usages have changed
            g_ct_on_usg->clear();
            for (auto it_ct : *g_ct->getCT())
                g_ct_on_usg->insert(it_ct);

            //add CTxCT to candidates
            g_cand->clear();
            end_ct = g_ct_on_usg->end();
            begin_ct = g_ct_on_usg->begin();
#ifdef FMP
            delete candidateOrder;
            candidateOrder = new priority_queue<pair<int, pair<DittoPattern*, DittoPattern*>>>();
            for (auto it1 = (g_ct->getCT())->begin(); it1 != g_ct->getCT()->end(); ++it1) {
                for (auto it2 = it1; it2 != g_ct->getCT()->end(); ++it2) {
                    candidateOrder->push(make_pair((*P_PTable::table)[P_PTable::patternIDMap[*it1]][P_PTable::patternIDMap[*it2]],
                                                   make_pair(*it1, *it2)));
                }
            }
#else
            it_ct_1 = codeTableSet::iterator(begin_ct);            //hard copy
            it_ct_2 = codeTableSet::iterator(begin_ct);            //hard copy
#endif

            par->cntAcc++;
            g_outputStream << "top accepted: ";
            g_outputStream << top->print(false);//DEBUG
        } else {
            par->cntRej++;
            g_ct->deleteDittoPattern(top);
            g_ct->rollback();            //we need to rollback because all usages must be correct before we can generate more candidates
#ifdef FMP
            P_PTable::rollbackTable();
#endif
        }
#ifdef FMP
//        cout << "P_PTable::total_p_id: " << P_PTable::total_p_id << endl;
#endif

    }


    g_ct->deleteUnusedDittoPatterns();            //Only at the very end
    g_cover = new Cover(par->seq, g_ct, false);

    //compute breakdown results for found patterns
    if (!par->dummy_file.empty()) {
        for (auto ct_it : *g_ct->getCT()) {
            if (ct_it->getSize() != 1)            //only consider non-singletons
                breakDown(ct_it);
        }

        //print the dummies for debug purpose
        for (int i = 0; i < par->nrOfDittoPatterns; ++i)
            g_outputStream << "dummy " << i << ": " << par->dummies[i]->toString() << endl;
    }

    g_outputStream << g_ct->printCT(false);    //NOTE: after search for dummy patterns

    par->lengthCT = g_ct->getCTLength();
    par->nr_non_singletons = g_ct->getCTLength() - par->alphabetSize;
    par->nr_non_singletons_per_size = g_ct->get_nr_non_singletons_per_size();

    par->STsize = STsize;
    par->CTsize = g_cover->get_szSequenceAndCT();
    par->perc = par->CTsize / par->STsize;
    par->eind = time(nullptr);

    FILE *pFile = nullptr;
    if (par->FPwindows) {
        //print FPwindows to file
        cout << "output" << endl;
        FILE *fpFile = nullptr;
        int cnt_fp = 0;
        fpFile = fopen((par->outputFilename + "_FP_windows.txt").c_str(), "w");
        if (fpFile != nullptr) {
            stringstream result, result2;
            for (auto p : *g_ct->getCT()) {
                if (p->getUsage() > 0 && p->getSize() > 1)        //all used non-singletons
                {
                    result << p->printFPwindows();
                    cnt_fp++;
                }
            }
            result2 << cnt_fp << "\n" << result.str();
            fprintf(fpFile, "%s", result2.str().c_str());
            fclose(fpFile);
        }
    }
    if (par->release) {
        //print output data to file
        pFile = fopen((par->outputFilename + ".txt").c_str(), "w");
        if (pFile != nullptr) {
            if (par->runtimes)
                g_outputStream << par->print();
            fprintf(pFile, "%s", g_outputStream.str().c_str());
            fclose(pFile);
        }
    } else {
        cout << g_outputStream.str();
        if (par->runtimes)
            cout << par->print();
    }

    if (par->runtimes) {
        bool exist = false;
        if (FILE *testFile = fopen("runtimes.csv", "r")) {
            fclose(testFile);
            exist = true;
        }

        FILE *runFile = fopen("runtimes.csv", "a");
        if (runFile != nullptr) {
            if (!exist)
                fprintf(runFile, "%s", par->header().c_str());

            fprintf(runFile, "%s", par->toString().c_str());
            fclose(runFile);
        }
    }

}


void Ditto::breakDown(DittoPattern *p) {
    //FOR EXACT MATCH - loop through all dummies
    bool exact = false;
    for (int i = 0; i < par->nrOfDittoPatterns; ++i) {
        dummy *d = par->dummies[i];
        exact = true;

        if (d->length != p->getLength() || d->size != p->getSize())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->getHeightAtPos(l) != d->events[l]->size())        //different height at timestep l
                exact = false;
            else {
                for (auto it = p->getSymbols(l)->begin(), end = p->getSymbols(l)->end(); it != end; ++it) {
                    if (!d->containsAttrSym(l, (*it)->attribute, (*it)->symbol)) {
                        exact = false;
                        break;
                    }
                }
            }
            if (!exact)
                break;
        }
        if (exact) {
            stringstream ss;
            ss << i;
            p->set_info("Exact " + ss.str());
            par->cntExact++;
            return;
        }
    }

    //FOR SUBSET MATCH -> alle multi-events van het patroon komen in dezelfde volgorde voor in een dummy-patroon
    for (int i = 0; i < par->nrOfDittoPatterns; ++i) {
        dummy *d = par->dummies[i];
        int dummy_ts = 0;
        bool subset;
        //every timestep must be found completely in the same order
        //loop through all timesteps in the pattern
        for (int l = 0; l < p->getLength(); ++l) {
            subset = false;
            while (dummy_ts < d->length) {
                bool completeTimestep = true;
                eventSet *evs = p->getSymbols(l);
                for (auto ev : *evs) {
                    bool completeSymbol = false;
                    attrSymSet *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            completeSymbol = true;
                            break;
                        }
                    if (!completeSymbol) {
                        completeTimestep = false;
                        break;        //try next timestep in dummy
                    }
                }
                dummy_ts++;
                if (completeTimestep) {
                    subset = true;
                    break;
                }
            }
            if (!subset)
                break;
        }

        if (subset) {
            stringstream ss;
            ss << i;
            p->set_info("Subset of " + ss.str());
            par->cntSubset++;
            return;
        }
    }

    //FOR UNION_SUBSET MATCH -> alle events komen in ��n dummy in verkeerde volgorde voor OF alle events komen in een (verschillende) dummy voor
    bool union_subset = true;
    for (int l = 0; l < p->getLength(); ++l) {
        eventSet *evs = p->getSymbols(l);
        for (auto ev : *evs) {
            //check for every event in the pattern if it occurs in one of the dummies
            bool foundEvent = false;
            for (int i = 0; i < par->nrOfDittoPatterns; ++i) {
                //loop over all dummies to find this event
                dummy *d = par->dummies[i];
                for (int dummy_ts = 0; dummy_ts < d->length; ++dummy_ts) {
                    attrSymSet *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            foundEvent = true;
                            break;
                        }
                    if (foundEvent)
                        break;
                }
                if (foundEvent)
                    break;
            }
            if (!foundEvent) {
                //this event is not found in all dummies
                union_subset = false;
                break;
            }
        }
        if (!union_subset)
            break;
    }
    if (union_subset) {
        p->set_info("Union Subset");
        par->cntUnion_subset++;
        return;
    }

    //if not exact or subset or union_subset it is unrelated
    p->set_info("Unrelated");
    par->cntUnrelated++;

}

//Return true when p is one of the inserted synthetic patterns
bool Ditto::checkIfDummy(DittoPattern *p) {
    bool match;
    for (int i = 0; i < par->nrOfDittoPatterns; ++i) {
        dummy *d = par->dummies[i];
        match = true;

        if (d->length != p->getLength() || d->size != p->getSize())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->getHeightAtPos(l) != d->events[l]->size())        //different height at timestep l
                match = false;
            else
                for (auto it = p->getSymbols(l)->begin(), end = p->getSymbols(l)->end(); it != end; ++it) {
                    if (!d->containsAttrSym(l, (*it)->attribute, (*it)->symbol)) {
                        match = false;
                        break;
                    }
                }
            if (!match) break;
        }
        if (match) return true;
    }
    return false;
}

void Ditto::loadOrBuildMinWindows(DittoPattern *p) {
    bool computeMinWindows = true;
    if (par->blacklist) {
        auto fnd = g_blackList->find(p), end_black = g_blackList->end();
        if (fnd != end_black) {
            p->loadWindowsAndSupport((*fnd));
            computeMinWindows = false;
        }
    }
    if (computeMinWindows && par->whitelist) {
        auto fnd = g_whiteList->find(p), end_white = g_whiteList->end();
        if (fnd != end_white) {
            p->loadWindowsAndSupport((*fnd));
            computeMinWindows = false;
        }
    }
    if (computeMinWindows) {
        par->cntMatPat++;

        p->setMinWindows(
                par->seq);                    //only compute the minWindows when we actually consider the pattern

        if (p->getSupport() < par->minsup)
            par->cntInfreqMaterialized++;

        if (par->prune_tree && p->getSupport() < par->minsup)
            g_root->addInfrequentDittoPattern(p, p->getSymbols(0)->begin(), p->getSymbols(0)->end());

        if (par->whitelist)
            g_whiteList->insert(p);                    //so we never have to compute its support and minWindows again
        if (par->blacklist && p->getSupport() == 0)
            g_blackList->insert(p);                    //so we never have to compute its support and minWindows again
    }
}


usgSz *Ditto::tryVariations(DittoPattern *accepted, usgSz *current_usgSz) {
    auto *tempCand = new patternSet;                    //so we don't try the same variation more often for the same pattern
    auto temp_end = tempCand->end();

    Multi_event **mev_time = par->seq->get_mev_time();

    int new_length = accepted->getLength() + 1;

    list<Window *> *minWin = accepted->getMinWindows(par->seq, false);

    for (auto w : *minWin) { //loop over its minimum windows
        if (w->active && w->get_GapLength() > 0) { //minwindows hebben max patternlength-1 gaps
            int startID = w->get_mev_position(0)->id;
            set<int> *gaps = w->get_gaps();

            for (int gap : *gaps) { //loop over all gap-positions
                eventSet *gapEvents = mev_time[gap]->getEvents();
                for (auto ev : *gapEvents) { //loop over all events at the gap-position               {
                    int gapCnt = 0;
                    int gapID = mev_time[gap]->id;
                    int gapPosition = 0;
                    for (int l = 0; l < accepted->getLength(); ++l) {
                        if (w->get_mev_position(l)->id < gapID)
                            gapPosition++;
                        else
                            break;
                    }
                    auto **eventSets = new eventSet *[new_length];
                    for (int l = 0; l < new_length; ++l) {
                        if (l == gapPosition) {
                            gapCnt = 1;
                            eventSets[l] = new eventSet;
                            eventSets[l]->insert(ev);
                        } else
                            eventSets[l] = new eventSet(*accepted->getSymbols(l - gapCnt)); //hard copy
                    }
                    auto *newp = new DittoPattern(new_length, eventSets, par->seq, accepted,
                                                  nullptr);        //we call this constructor because it doesn't build the minWindows yet
                    //prune on minsup
                    bool deleteDittoPattern = false;
                    if (par->prune_tree) {
                        //findDittoPattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
                        for (int startPos = 0; startPos < newp->getLength(); ++startPos) {
                            if (g_root->findDittoPattern(newp, startPos, newp->getSymbols(0)->begin(),
                                                         newp->getSymbols(0)->end())) {
                                par->cntInfreq++;
                                deleteDittoPattern = true;
                                break;        //if the pattern from one of the startPositions is found then we know enough
                            }
                        }
                    }

                    if (!deleteDittoPattern && findDittoPatternInSet(tempCand, newp) ==
                                               temp_end)                                //only when not already tried
                    {
                        tempCand->insert(newp);
                        loadOrBuildMinWindows(newp);

                        if (newp->getSupport() < par->minsup) {
                            if (par->prune_tree)//add to prune tree
                                g_root->addInfrequentDittoPattern(newp, newp->getSymbols(0)->begin(),
                                                                  newp->getSymbols(0)->end());
                        } else {
                            if (g_ct->insertDittoPattern(
                                    newp))                    //NOTE this check must be after setMinWindows, because pattern equality is also based on support
                            {
                                g_cover = new Cover(par->seq, g_ct, false);
                                double new_size = g_cover->get_szSequenceAndCT();
                                int new_total_usage = g_cover->get_totalUsage();
                                if (new_size < current_usgSz->sz)                                    //Check improvement
                                {
                                    par->cntAccVar++;
                                    current_usgSz = postprune(newp, new_total_usage,
                                                              new_size);        //post acceptance pruning
                                    current_usgSz = tryVariations(newp,
                                                                  current_usgSz);                //recursively try variations of newp+singleton
                                } else {
                                    par->cntRejVar++;
                                    g_ct->deleteDittoPattern(newp);
#ifdef FMP
//                                    --P_PTable::total_p_id;
#endif
                                }
                            }
#ifdef FMP
//                            else {
//                                --P_PTable::total_p_id;
//                            }
#endif
                        }
                    } else
                        delete newp;
                }
            }
        }
    }

    delete tempCand;
    return current_usgSz;
}
#ifdef FMP
void Ditto::generateCandidates(usgSz *current_usgSz) {
    bool stop = false;
    while (!candidateOrder->empty()) {
        auto top = candidateOrder->top();
        if (top.first < candThreshold) {
            break;
        }
        DittoPattern * p1 = top.second.first;
        DittoPattern * p2 = top.second.second;
        if ((p1)->getSupport() < par->minsup) {
            candidateOrder->pop();
            continue;
        }
        if ((p2)->getSupport() < par->minsup) {
            candidateOrder->pop();
            continue;
        }

        auto *result = new patternSet;
        stop = combineDittoPatterns(p1, p2, current_usgSz->usg, result);
        if (stop)
            break;

        insertCandidates(result);
        candidateOrder->pop();
    }
}
#else
void Ditto::generateCandidates(usagepatternSet::iterator *pt_ct_1, usagepatternSet::iterator *pt_ct_2,
                               usagepatternSet::iterator *pt_begin_ct, usagepatternSet::iterator *pt_end_ct,
                               usgSz *current_usgSz) {
    //order: 1x1, 1x2, 2x2, 1x3, 2x3, 3x3, 1x4, 2x4, 3x4, 4x4, 1x5, ..
    bool stop = false;
    while (*pt_ct_2 != *pt_end_ct)        //it_ct_2 is the first to reach the end
    {
        //if one of the patterns is a fill-pattern we do not combine it
        if (par->fillDittoPatterns) {
            if ((**pt_ct_1)->getSize() == 1 &&
                par->fillDittoPattern[(*(**pt_ct_1)->getSymbols(0)->begin())->attribute] &&
                (*(**pt_ct_1)->getSymbols(0)->begin())->symbol ==
                par->alphabetSizes[(*(**pt_ct_1)->getSymbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_1;
                continue;
            }
            if ((**pt_ct_2)->getSize() == 1 &&
                par->fillDittoPattern[(*(**pt_ct_2)->getSymbols(0)->begin())->attribute] &&
                (*(**pt_ct_2)->getSymbols(0)->begin())->symbol ==
                par->alphabetSizes[(*(**pt_ct_2)->getSymbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_2;
                continue;
            }
        }

        //if one of the patterns support is below minsup we do not combine
        if ((**pt_ct_1)->getSupport() < par->minsup) {
            ++*pt_ct_1;
            continue;
        }
        if ((**pt_ct_2)->getSupport() < par->minsup) {
            ++*pt_ct_2;
            continue;
        }

        auto *result = new patternSet;
        stop = combineDittoPatterns(**pt_ct_1, **pt_ct_2, current_usgSz->usg, result);
        if (stop)
            break;

        insertCandidates(result);

        if (*pt_ct_1 == *pt_ct_2) {
            *pt_ct_1 = usagepatternSet::iterator(*pt_begin_ct);    //hard copy
            ++*pt_ct_2;
        } else
            ++*pt_ct_1;
    }

}
#endif

//Only adds the patterns if they are not already present
void Ditto::insertCandidates(patternSet *plist) {
    auto it = plist->begin(), end = plist->end();
    while (it != end) {
        DittoPattern *p = *it;
        bool deleteDittoPattern = false;

        if (par->prune_tree) {
            //findDittoPattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
            for (int startPos = 0; startPos < p->getLength(); ++startPos) {
                if (g_root->findDittoPattern(p, startPos, p->getSymbols(startPos)->begin(),
                                             p->getSymbols(startPos)->end())) {
                    par->cntInfreq++;
                    deleteDittoPattern = true;
                    break;        //if the pattern from one of the startPositions is found then we know enough
                }
            }
        }
        if (!deleteDittoPattern) {
            auto candidate_end = g_cand->end();
            if (findDittoPatternInSet(g_cand, p) == candidate_end)
                g_cand->insert(p);
            else
                deleteDittoPattern = true;
        }
        if (deleteDittoPattern) {

            delete p;
        }
        ++it;
    }
}


//Returns true when we do not combine x and y because of their usage compared to bestCand
//Returns a list of patterns constructed from a and b via the patternSet* result
bool Ditto::combineDittoPatterns(DittoPattern *x, DittoPattern *y, int usgS, patternSet *result) {
    if (!g_cand->empty() && (x->getUsage() < (*g_cand->begin())->getEstimatedUsage() ||
                             y->getUsage() < (*g_cand->begin())->getEstimatedUsage()))
        return true;

    int usgZ, usgX, usgY;

    //check whether they specify values for a similar attribute
    bool simAttr = false;
    set<int> *setX = x->getTotalAIDs(), *setY = y->getTotalAIDs();
    auto endY = setY->end();
    for (int it : *setX) {
        if (setY->find(it) != endY) {
            simAttr = true;
            break;
        }
    }

    for (int offset = 0; offset < x->getLength() + y->getLength() + 1; ++offset) {
        if (!simAttr || !checkDittoPatternAttributeOverlap(x, y, offset)) {
            DittoPattern *newp = buildDittoPattern(x, y, offset);
            usgX = newp->getX()->getUsage();
            usgY = newp->getY()->getUsage();
            usgZ = min(usgX, usgY);                        //estimated usage
            newp->setEstimatedGain(newp->computeEstimatedGain(usgX, usgY, usgZ, usgS));
            result->insert(newp);
        }
    }

    return false;
}

//RETURN true when eventsets a and b contain a similar attribute
bool Ditto::checkEventsetAttributeOverlap(eventSet *a, eventSet *b) {
    bool result = false;
    bool *presentAttributes = new bool[par->nrOfAttributes];
    for (int i = 0; i < par->nrOfAttributes; ++i)
        presentAttributes[i] = false;

    for (auto ita : *a)  //one eventset can not contain duplicate attributes
        presentAttributes[ita->attribute] = true;
    for (auto itb : *b) {
        if (presentAttributes[itb->attribute]) {  //check if the attribute was already seen in 'a'
            result = true;
            break;
        }
        presentAttributes[itb->attribute] = true;
    }
    delete[]presentAttributes;
    return result;
}

//RETURN true when DittoPatterns a and b overlap when combined with the given offset
bool Ditto::checkDittoPatternAttributeOverlap(DittoPattern *a, DittoPattern *b,
                                              int offset) {    //	  Offset:	0		1		2		3		4		5
    // DittoPattern A:   xxx		xxx		xxx		xxx		xxx		xxx
    // DittoPattern B: xx	   xx		xx		 xx		  xx	   xx
    //	  length:	5		4		3		3		4		5
    // startPosA:	2		1		0		0		0		0
    // startPosB:	0		0		0		1		2		3

    //	  Offset:	0		1		2		3
    // DittoPattern A:   x		x		x		x
    // DittoPattern B: xx	   xx		xx		 xx
    //	  length:	3		2		2		3
    // startPosA:	2		1		0		0
    // startPosB:	0		0		0		1

    //	  Offset:	0		1		2		3
    // DittoPattern A:   xx		xx		xx 		xx
    // DittoPattern B:  x	    x		 x		  x
    //	  length:	3		2		2		3
    // startPosA:	1		0		0		0
    // startPosB:	0		0		1		2


    if (offset == 0 || offset == a->getLength() + b->getLength())    //one pattern completely before the other
        return false;

    int startPosA = max(0, b->getLength() - offset), startPosB = max(0, offset - b->getLength());
    int newLength;
    if (startPosB == 0)
        newLength = max(startPosA + a->getLength(), b->getLength());
    else
        newLength = max(startPosB + b->getLength(), a->getLength());
    for (int pos = 0; pos < newLength; ++pos) {
        if (pos >= startPosA && pos >= startPosB && pos - startPosA < a->getLength() &&
            pos - startPosB < b->getLength())
            if (checkEventsetAttributeOverlap(a->getSymbols(pos - startPosA), b->getSymbols(pos - startPosB)))
                return true;
    }
    return false;
}

eventSet *Ditto::joinEventsets(eventSet *a, eventSet *b) {
    auto *result = new eventSet;
    for (auto ita : *a)
        result->insert(ita);
    for (auto itb : *b)
        result->insert(itb);
    return result;
}

//construct a new pattern from a singleton and another pattern, with the singleton at the specified position
DittoPattern *Ditto::buildInterleavedDittoPattern(DittoPattern *singleton, DittoPattern *p, int pos_singleton) {
    int newLength = p->getLength() + 1;
    int pos_p = 0;
    auto **eventSets = new eventSet *[newLength];
    for (int pos = 0; pos < newLength; ++pos) {
        if (pos == pos_singleton)
            eventSets[pos] = new eventSet(*singleton->getSymbols(0)); //hard copy
        else
            eventSets[pos] = new eventSet(*p->getSymbols(pos_p++)); //hard copy
    }
    return new DittoPattern(newLength, eventSets, par->seq, singleton, p);
}

//construct a new pattern from a and b with given offset (already checked that there is no overlap)
DittoPattern *Ditto::buildDittoPattern(DittoPattern *a, DittoPattern *b, int offset) {
    DittoPattern *newp;

    int startPosA = max(0, b->getLength() - offset), startPosB = max(0, offset - b->getLength());
    int newLength;
    if (startPosB == 0)
        newLength = max(startPosA + a->getLength(), b->getLength());
    else
        newLength = max(startPosB + b->getLength(), a->getLength());

    auto **eventSets = new eventSet *[newLength];
    for (int pos = 0; pos < newLength; ++pos) {
        if (pos >= startPosA && pos >= startPosB && pos - startPosA < a->getLength() &&
            pos - startPosB < b->getLength())
            eventSets[pos] = joinEventsets(a->getSymbols(pos - startPosA), b->getSymbols(pos - startPosB));
        else if (pos >= startPosA && pos - startPosA < a->getLength())
            eventSets[pos] = new eventSet(*a->getSymbols(pos - startPosA)); //hard copy
        else if (pos >= startPosB && pos - startPosB < b->getLength())
            eventSets[pos] = new eventSet(*b->getSymbols(pos - startPosB)); //hard copy
    }
    newp = new DittoPattern(newLength, eventSets, par->seq, a, b);
    return newp;
}


usgSz *Ditto::postprune(DittoPattern *accepted, int current_total_usg, double current_size) {
    auto *pruneset = new prunepatternSet;
    for (auto p : *g_ct->getCT()) {
        if (p->getSize() > 1)//singletons can't be pruned
            if (p->getUsageDecreased())
                pruneset->insert(p);
    }
    while (!pruneset->empty()) {
        //prune the top
        auto it_top = pruneset->begin();
        DittoPattern *top = *it_top;

        pruneset->erase(it_top);        //erase on iterator, because pruneset compares on usage
        g_ct->deleteDittoPattern(top);

        g_cover = new Cover(par->seq, g_ct, false);

        double new_size = g_cover->get_szSequenceAndCT();
        if (new_size < current_size)            //Check improvement
        {
            //check if pruned pattern was subset of last accepted pattern
            if (par->pruneCheck) {
                if (!checkSubset(top, accepted)) {
                    g_outputStream << "\nPruned a pattern that was not a subset of the accepted pattern.\n"
                                   << "\tAccepted: " << accepted->print(false) << "\tPruned: " << top->print(false)
                                   << endl;
                }
            }
            current_total_usg = g_cover->get_totalUsage();
            current_size = new_size;
            //add more prune candidates
            for (auto p : *g_ct->getCT()) {
                if (p->getSize() > 1 && p->getUsageDecreased())                //singletons can't be pruned
                    if (findDittoPatternInSet(pruneset, p) != pruneset->end())    //if not already present
                        pruneset->insert(p);
            }
        } else {
            g_ct->rollback();
            g_ct->insertDittoPattern(top);        //put pattern back
        }
    }
    delete pruneset;
    return new usgSz(current_total_usg, current_size);
}

//checks whether esa is a subset of esb
bool Ditto::checkSubset(eventSet *esa, eventSet *esb) {
    auto endb = esb->end();
    for (auto it : *esa)
        if (esb->find(it) == endb)
            return false;
    return true;
}

//checks whether a is a subset of b
bool Ditto::checkSubset(DittoPattern *a, DittoPattern *b) {
    int la = 0;
    for (int lb = 0; lb < b->getLength(); ++lb) {
        if (checkSubset(a->getSymbols(la), b->getSymbols(lb)))
            la++;

        if (la == a->getLength())
            return true;
    }
    return false;
}


candpatternSet::iterator Ditto::findDittoPatternInSet(patternSet *pset, DittoPattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

candpatternSet::iterator Ditto::findDittoPatternInSet(candpatternSet *pset, DittoPattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

prunepatternSet::iterator Ditto::findDittoPatternInSet(prunepatternSet *pset, DittoPattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

Ditto::~Ditto() {
    delete par->seq;
    delete g_ct;
    delete g_cand;
    delete g_ct_on_usg;
    delete g_cover;
    if (g_whiteList)
        g_whiteList->clear();
    delete g_whiteList;
    delete g_blackList;
}
