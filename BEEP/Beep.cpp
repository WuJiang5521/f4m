#include "Common.h"
#include "Beep.h"

using namespace std;

int enter_beep(int argc, char **argv) {
    Parameters parameters;
    parameters.release = true;

    parameters.runtimes = true;
    parameters.debug = false;
    parameters.debug2 = false;

    parameters.gapvariants = true;
    parameters.prune_check = false;            //mag eruit
    parameters.blacklist = false;            //mag eruit
    parameters.whitelist = false;
    parameters.prune_est_gain = true;
    parameters.prune_tree = true;
    parameters.FP_windows = false;            //to print the found patterns in the data AND to translate patterns from text-data
    parameters.fill_patterns = false;        //for aligned text-data

    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    parameters.cnt_covers = 0;
    parameters.cnt_mat_pat = 0;
    parameters.cnt_acc = 0;
    parameters.cnt_rej = 0;
    parameters.cnt_acc_var = 0;
    parameters.cnt_rej_var = 0;
    parameters.cnt_infreq_materialized = 0;
    parameters.cnt_infreq = 0;

    parameters.minsup = 5;
    parameters.dummy_file = "";
    parameters.input_filename = "";

    if (parameters.release) {
        //DEFAULT values
        parameters.minsup = 5;
        parameters.dummy_file = "";
        parameters.input_filename = "";

        char opt_pattern[] = "i:t:m:p:w:f:";
        int c, pos;
        while ((c = get_opt(argc, argv, opt_pattern)) != -1) {
            switch (c) {
                case 'w':
                    parameters.FP_windows = (atoi(poptarg) != 0);
                    break;
                case 'f':
                    parameters.fill_patterns = (atoi(poptarg) != 0);
                    break;
                case 'i':
                    parameters.input_filename = string(poptarg);
                    pos = parameters.input_filename.find_last_of('.');
                    if (pos != parameters.input_filename.length() - 4 ||
                        parameters.input_filename.substr(pos, parameters.input_filename.length() - 1) != ".dat") {
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
                         << "\t-w\ttrue=print all found patterns to file (also used to translate patterns from text-data)\n"
                         << "\t-f\ttrue=fill-patterns are used to aligne text-data\n";
                    return 1;
            }
        }
        if (parameters.input_filename.empty()) {
            cout << "ERROR: <inputfile>.dat is mandatory, type -h for help.\n";
            return 1;
        }
    }

    FILE *f;
    if (!(f = fopen(parameters.input_filename.c_str(), "r"))) {
        cout << "ERROR opening data file: " << parameters.input_filename << "\n";
        if (!parameters.release)
            system("pause");
        return 1;
    }
    parameters.seq = new Sequence(f, &parameters);
    fclose(f);

    if (parameters.seq->error_flag) {
        cout << "ERROR reading sequence or patterns!\n";
        if (!parameters.release)
            system("pause");
        return 1;
    }

    stringstream ss;
    ss << parameters.minsup;
    int strt = parameters.input_filename.find_last_of("/\\") + 1, nd = parameters.input_filename.find_last_of('.');
    string temp = parameters.input_filename.substr(strt, nd - strt);
    parameters.output_filename = "output_fileData_" + temp + "_minsup" + ss.str();

    if (parameters.fill_patterns)    //to make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)
    {
        parameters.fill_pattern = new bool[parameters.nr_of_attributes];
        for (int attr = 0; attr < parameters.nr_of_attributes; ++attr)
            parameters.fill_pattern[attr] = true;
    }
    clock_t t0 = clock();
    auto *beep = new Beep(&parameters);
    cout << "BEEP time: " << (clock() - t0)*1.0/CLOCKS_PER_SEC << "s" << endl;
    delete beep;

    if (!parameters.release)
        system("pause");
    return 0;
}
#ifdef MISS
bool miss_print_debug = false;
std::ofstream outfile_miss;
#endif
Beep::Beep(Parameters *par) : par(par) {
#ifdef MISS
    outfile_miss.open("./miss_debug_output.txt");
#endif
    output_stream << par->seq->print_sequence(false);      //DEBUG

    par->start = time(nullptr);
    par->cnt_exact = 0;
    par->cnt_subset = 0;
    par->cnt_union_subset = 0;
    par->cnt_unrelated = 0;

    //build a code_table
    ct = new CodeTable(par->seq);
    black_list = new pattern_set;
    white_list = new pattern_set;
    ct_on_usg = new usagepattern_set;        //set to combine CTxCT based on usage
#ifdef LSH
    candidate_order = new priority_queue<pair<int, pair<Pattern*, Pattern*>>>();
#endif

    //build singletons
    int **tree_ids = par->seq->get_tree_ids();
    int nr_singletons = par->alphabet_size;
    int *alphabet_sizes = par->alphabet_sizes;
    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        for (int sym = 0; sym < alphabet_sizes[aid]; ++sym) {
            auto **event_sets = new event_set *[1];
            event_sets[0] = new event_set;
            event_sets[0]->insert(new Attribute(sym, aid, 0, tree_ids[aid][sym]));
            auto *p = new Pattern(1, event_sets, par->seq);
            ct->insert_pattern(p);
            ct_on_usg->insert(p);
        }
    }
#ifdef LSH
    vector<Pattern*> singletons;
    PatternTable::pattern_id_map.clear();
    int initial_pattern_id_map_top = 0;
    for (auto & p : *(ct->get_ct())) {
        singletons.push_back(p);
        PatternTable::pattern_id_map[p] = initial_pattern_id_map_top++;
        PatternTable::pattern_id_table.push_back(p);
    }

    PatternTable::table_size = initial_pattern_id_map_top;
    PatternTable::pre_table.resize(PatternTable::table_size);
    PatternTable::last_table.resize(PatternTable::table_size);
    for (int i = 0; i < PatternTable::table_size; ++i) {
        PatternTable::pre_table[i].resize(PatternTable::table_size, 0);
        PatternTable::last_table[i].resize(PatternTable::table_size, 0);
    }
#endif

    //build a tree that represents all patterns of length == 1
    //each timestep in a new candidate is run through this tree to see if it still can be frequent
    root = new Node(nr_singletons, par->alphabet_sizes, 0);

    cand = new candpattern_set;    //NOTE: candidates are ordered based on estimated gain

    auto it_ct_1 = ct_on_usg->begin(), it_ct_2 = ct_on_usg->begin(), begin_ct = ct_on_usg->begin(), end_ct = ct_on_usg->end();    //iterators to update candidate list

    auto *g_cover = new Cover(par->seq, ct, false);    //determine ST size
#ifdef LSH
    delete candidate_order;
    candidate_order = new priority_queue<pair<int, pair<Pattern*, Pattern*>>>();
    for (int i = 0; i < (*PatternTable::table).size(); ++i) {
        for (int j = 0; j < (*PatternTable::table)[i].size(); ++j) {
            candidate_order->push(make_pair((*PatternTable::table)[i][j], make_pair(PatternTable::pattern_id_table[i], PatternTable::pattern_id_table[j])));
        }
    }
#endif

    auto *current_usgSz = new usg_sz(g_cover->get_total_usage(), g_cover->get_sz_sequence_and_ct());
    double init_sz = current_usgSz->sz;

    double STsize = current_usgSz->sz;
    output_stream << "\n\nST " << *current_usgSz << endl;//DEBUG
    output_stream << ct->print_ct(false);//DEBUG

    while (true) {
#ifdef LSH
        if ((candidate_order->empty() || candidate_order->top().first < cand_threshold * par->seq->get_nr_sequences()) &&
            cand->empty())                //we stop when there are no more candidates to generate. NOTE it_ct_2 reaches end first
            break;
        generate_candidates(current_usgSz);
#else
        if (it_ct_2 == end_ct &&
        cand->empty())                //we stop when there are no more candidates to generate. NOTE it_ct_2 reaches end first
            break;
        //update candidates
        generate_candidates(&it_ct_1, &it_ct_2, &begin_ct, &end_ct, current_usgSz);
#endif
//        cout << "*****" << endl;
        if (cand->empty())
            continue;

        //Add top candidate
        Pattern *top = *cand->begin();
        cand->erase(
                top);                            //do not consider this candidate again. NOTE: it must also be erased from cand to not be compared to as best candidate

        if (par->prune_est_gain && top->get_estimated_gain() <= 0)
            continue;

        load_or_build_min_windows(top);

        //prune on minsup
        if (top->get_support() < par->minsup)
            continue;

        if (!ct->insert_pattern(
                top))                //pattern already present. NOTE this check must be after set_min_windows, because pattern equality is also based on support
            continue;                                //already present
#ifdef LSH
        PatternTable::check_table();
        int pattern_id_map_top = 0;
        PatternTable::pattern_id_table.clear();
        PatternTable::pattern_id_map.clear();
        for (auto & p : *(ct->get_ct())) {
            PatternTable::pattern_id_map[p] = pattern_id_map_top++;
            PatternTable::pattern_id_table.push_back(p);
        }

        PatternTable::table_size = pattern_id_map_top;
        if (PatternTable::table_size > PatternTable::pre_table.size()) {
            PatternTable::pre_table.resize(PatternTable::table_size);
            PatternTable::last_table.resize(PatternTable::table_size);
            for (int i = 0; i < PatternTable::table_size; ++i) {
                PatternTable::pre_table[i].resize(PatternTable::table_size, 0);
                PatternTable::last_table[i].resize(PatternTable::table_size, 0);
            }
        }
        PatternTable::clear_table(PatternTable::table_size);
#endif
        //Cover
        g_cover = new Cover(par->seq, ct, false);
        double new_size = g_cover->get_sz_sequence_and_ct();
        int new_total_usage = g_cover->get_total_usage();

//        cout << "current_size = " << current_usgSz->sz << "  new_size = " << new_size << " dL: " << (init_sz - current_usgSz->sz) / init_sz * 100.0 << endl;//DEBUG
        //Check improvement
        if (new_size < current_usgSz->sz) {
            current_usgSz = postprune(top, new_total_usage, new_size);                //post acceptance pruning
            if (par->gapvariants)                                                    //recursively try variations of top+singleton
                current_usgSz = try_variations(top, current_usgSz);

            //rebuild ct_on_usg because usages have changed
            ct_on_usg->clear();
            for (auto it_ct : *ct->get_ct())
                ct_on_usg->insert(it_ct);

            //add CTxCT to candidates
            cand->clear();
            end_ct = ct_on_usg->end();
            begin_ct = ct_on_usg->begin();
#ifdef LSH
            delete candidate_order;
            candidate_order = new priority_queue<pair<int, pair<Pattern*, Pattern*>>>();
            for (auto it1 = (ct->get_ct())->begin(); it1 != ct->get_ct()->end(); ++it1) {
                for (auto it2 = it1; it2 != ct->get_ct()->end(); ++it2) {
                    Pattern *minp = *it1 > *it2 ? *it2 : *it1;
                    Pattern *maxp = *it1 <= *it2 ? *it2 : *it1;
                    candidate_order->push(make_pair((*PatternTable::table)[PatternTable::pattern_id_map[minp]][PatternTable::pattern_id_map[maxp]],
                                                    make_pair(minp, maxp)));
                }
            }
#else
            it_ct_1 = codeTable_set::iterator(begin_ct);            //hard copy
            it_ct_2 = codeTable_set::iterator(begin_ct);            //hard copy
#endif

            par->cnt_acc++;
            output_stream << "top accepted: ";
            output_stream << top->print(false);//DEBUG
        } else {
            par->cnt_rej++;
            ct->delete_pattern(top);
            ct->rollback();            //we need to rollback because all usages must be correct before we can generate more candidates
#ifdef LSH
            PatternTable::rollback_table();
#endif
        }

    }


    ct->delete_unused_patterns();            //Only at the very end
    g_cover = new Cover(par->seq, ct, false);

#ifdef MISS
    int debug_miss_cnt = 0;
    for (auto it_ct : *ct->get_ct()) {
        if (it_ct->get_usage() > 0 && it_ct->get_length() > 1) {
            debug_miss_cnt += it_ct->get_usage_miss();
        }
    }
    cout << "miss count: " << debug_miss_cnt << endl;
#endif
    cout << "nr seq: " << par->seq->get_nr_sequences() << endl;
    double s_len_avg = 0;
    for (int i = 0; i < par->seq->get_nr_sequences(); ++i) {
        s_len_avg += par->seq->get_sequence_sizes()[i];
    }
    cout << "seq len avg: " << s_len_avg / par->seq->get_nr_sequences() << endl;
    cout << "nr Attribute: " << par->nr_of_attributes << endl;
    int event_len = 0;
    for (int i = 0; i < par->nr_of_attributes; ++i) {
        event_len += par->alphabet_sizes[i];
    }
    cout << "event len: " << event_len << endl;
    int ct_no_singleton_cnt = 0, ct_len_avg = 0;
    for (auto & p : *(ct->get_ct())) {
        ct_len_avg += p->get_size();
        if (p->get_size() > 1) {
            ++ct_no_singleton_cnt;
        }
    }
    cout << "P num: " << ct_no_singleton_cnt << "P len avg: " << (double)ct_len_avg / ct->get_ct()->size() << endl;
    double now_L = g_cover->get_sz_sequence_and_ct();
#ifdef MISS
//        for (auto it_ct : *ct->get_ct()) {
//            if (it_ct->get_usage() > 0 && it_ct->get_length() > 1) {
//                now_L -= it_ct->get_usage_miss() *
//                        it_ct->get_codelength_miss();
//            }
//        }
#endif
    cout << "percent of delta L: " << (init_sz - now_L) / init_sz * 100.0 << endl;
    //compute breakdown results for found patterns
    if (!par->dummy_file.empty()) {
        for (auto ct_it : *ct->get_ct()) {
            if (ct_it->get_size() != 1)            //only consider non-singletons
                break_down(ct_it);
        }

        //print the dummies for debug purpose
        for (int i = 0; i < par->nr_of_patterns; ++i)
            output_stream << "Dummy " << i << ": " << par->dummies[i]->to_string() << endl;
    }

#ifdef MISS // DEBUG
//    miss_print_debug = true;
//    cover = new Cover(par->seq, ct, false);
#endif

    output_stream << ct->print_ct(false);    //NOTE: after search for Dummy patterns

    par->length_CT = ct->get_ct_length();
    par->nr_non_singletons = ct->get_ct_length() - par->alphabet_size;
    par->nr_non_singletons_per_size = ct->get_nr_non_singletons_per_size();

    par->STsize = STsize;
    par->CTsize = g_cover->get_sz_sequence_and_ct();
    par->perc = par->CTsize / par->STsize;
    par->eind = time(nullptr);

    FILE *p_file = nullptr;
    if (par->FP_windows) {
        //print FP_windows to file
        cout << "output" << endl;
        FILE *fp_file = nullptr;
        int cnt_fp = 0;
        fp_file = fopen((par->output_filename + "_FP_windows.txt").c_str(), "w");
        if (fp_file != nullptr) {
            stringstream result, result2;
            for (auto p : *ct->get_ct()) {
                if (p->get_usage() > 0 && p->get_size() > 1)        //all used non-singletons
                {
                    result << p->print_fp_windows();
                    cnt_fp++;
                }
            }
            result2 << cnt_fp << "\n" << result.str();
            fprintf(fp_file, "%s", result2.str().c_str());
            fclose(fp_file);
        }
    }
    if (par->release) {
        //print output data to file
        p_file = fopen((par->output_filename + ".txt").c_str(), "w");
        if (p_file != nullptr) {
            if (par->runtimes)
                output_stream << par->print();
            fprintf(p_file, "%s", output_stream.str().c_str());
            fclose(p_file);
        }
    } else {
        cout << output_stream.str();
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

            fprintf(runFile, "%s", par->to_string().c_str());
            fclose(runFile);
        }
    }

}


void Beep::break_down(Pattern *p) {
    //FOR EXACT MATCH - loop through all dummies
    bool exact = false;
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        exact = true;

        if (d->length != p->get_length() || d->size != p->get_size())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->get_height_at_pos(l) != d->events[l]->size())        //different height at timestep l
                exact = false;
            else {
                for (auto it = p->get_symbols(l)->begin(), end = p->get_symbols(l)->end(); it != end; ++it) {
                    if (!d->contains_attr_sym(l, (*it)->attribute, (*it)->symbol)) {
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
            par->cnt_exact++;
            return;
        }
    }

    //FOR SUBSET MATCH -> alle multi-events van het patroon komen in dezelfde volgorde voor in een Dummy-patroon
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        int dummy_ts = 0;
        bool subset;
        //every timestep must be found completely in the same order
        //loop through all timesteps in the pattern
        for (int l = 0; l < p->get_length(); ++l) {
            subset = false;
            while (dummy_ts < d->length) {
                bool complete_timestep = true;
                event_set *evs = p->get_symbols(l);
                for (auto ev : *evs) {
                    bool complete_symbol = false;
                    attr_sym_set *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            complete_symbol = true;
                            break;
                        }
                    if (!complete_symbol) {
                        complete_timestep = false;
                        break;        //try next timestep in Dummy
                    }
                }
                dummy_ts++;
                if (complete_timestep) {
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
            par->cnt_subset++;
            return;
        }
    }

    //FOR UNION_SUBSET MATCH -> alle events komen in ��n Dummy in verkeerde volgorde voor OF alle events komen in een (verschillende) Dummy voor
    bool union_subset = true;
    for (int l = 0; l < p->get_length(); ++l) {
        event_set *evs = p->get_symbols(l);
        for (auto ev : *evs) {
            //check for every event in the pattern if it occurs in one of the dummies
            bool found_event = false;
            for (int i = 0; i < par->nr_of_patterns; ++i) {
                //loop over all dummies to find this event
                Dummy *d = par->dummies[i];
                for (int dummy_ts = 0; dummy_ts < d->length; ++dummy_ts) {
                    attr_sym_set *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            found_event = true;
                            break;
                        }
                    if (found_event)
                        break;
                }
                if (found_event)
                    break;
            }
            if (!found_event) {
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
        par->cnt_union_subset++;
        return;
    }

    //if not exact or subset or union_subset it is unrelated
    p->set_info("Unrelated");
    par->cnt_unrelated++;

}

//Return true when p is one of the inserted synthetic patterns
bool Beep::check_if_dummy(Pattern *p) {
    bool match;
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        match = true;

        if (d->length != p->get_length() || d->size != p->get_size())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->get_height_at_pos(l) != d->events[l]->size())        //different height at timestep l
                match = false;
            else
                for (auto it = p->get_symbols(l)->begin(), end = p->get_symbols(l)->end(); it != end; ++it) {
                    if (!d->contains_attr_sym(l, (*it)->attribute, (*it)->symbol)) {
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

void Beep::load_or_build_min_windows(Pattern *p) {
    bool compute_min_windows = true;
    if (par->blacklist) {
        auto fnd = black_list->find(p), end_black = black_list->end();
        if (fnd != end_black) {
            p->load_windows_and_support((*fnd));
            compute_min_windows = false;
        }
    }
    if (compute_min_windows && par->whitelist) {
        auto fnd = white_list->find(p), end_white = white_list->end();
        if (fnd != end_white) {
            p->load_windows_and_support((*fnd));
            compute_min_windows = false;
        }
    }
    if (compute_min_windows) {
        par->cnt_mat_pat++;

        p->set_min_windows(
                par->seq);                    //only compute the min_windows when we actually consider the pattern

        if (p->get_support() < par->minsup)
            par->cnt_infreq_materialized++;

        if (par->prune_tree && p->get_support() < par->minsup)
            root->add_infrequent_pattern(p, p->get_symbols(0)->begin(), p->get_symbols(0)->end());

        if (par->whitelist)
            white_list->insert(p);                    //so we never have to compute its support and min_windows again
        if (par->blacklist && p->get_support() == 0)
            black_list->insert(p);                    //so we never have to compute its support and min_windows again
    }
}


usg_sz *Beep::try_variations(Pattern *accepted, usg_sz *current_usgSz) {
    auto *temp_cand = new pattern_set;                    //so we don't try the same variation more often for the same pattern
    auto temp_end = temp_cand->end();

    Event **mev_time = par->seq->get_mev_time();

    int new_length = accepted->get_length() + 1;

    list<Window *> *minWin = accepted->get_min_windows(par->seq, false);

    for (auto w : *minWin) { //loop over its minimum windows
        if (w->active && w->get_gap_length() > 0) { //minwindows hebben max patternlength-1 gaps
            int start_id = w->get_mev_position(0)->id;
            set<int> *gaps = w->get_gaps();

            for (int gap : *gaps) { //loop over all gap-positions
                event_set *gap_events = mev_time[gap]->get_events();
                for (auto ev : *gap_events) { //loop over all events at the gap-position               {
                    int gap_cnt = 0;
                    int gap_ID = mev_time[gap]->id;
                    int gap_position = 0;
                    for (int l = 0; l < accepted->get_length(); ++l) {
                        if (w->get_mev_position(l)->id < gap_ID)
                            gap_position++;
                        else
                            break;
                    }
                    auto **event_sets = new event_set *[new_length];
                    for (int l = 0; l < new_length; ++l) {
                        if (l == gap_position) {
                            gap_cnt = 1;
                            event_sets[l] = new event_set;
                            event_sets[l]->insert(ev);
                        } else
                            event_sets[l] = new event_set(*accepted->get_symbols(l - gap_cnt)); //hard copy
                    }
                    auto *newp = new Pattern(new_length, event_sets, par->seq, accepted,
                                             nullptr);        //we call this constructor because it doesn't build the min_windows yet
                    //prune on minsup
                    bool delete_pattern = false;
                    if (par->prune_tree) {
                        //find_pattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
                        for (int startPos = 0; startPos < newp->get_length(); ++startPos) {
                            if (root->find_pattern(newp, startPos, newp->get_symbols(0)->begin(),
                                                   newp->get_symbols(0)->end())) {
                                par->cnt_infreq++;
                                delete_pattern = true;
                                break;        //if the pattern from one of the startPositions is found then we know enough
                            }
                        }
                    }

                    if (!delete_pattern && find_pattern_in_set(temp_cand, newp) ==
                                           temp_end)                                //only when not already tried
                    {
                        temp_cand->insert(newp);
                        load_or_build_min_windows(newp);

                        if (newp->get_support() < par->minsup) {
                            if (par->prune_tree)//add to prune tree
                                root->add_infrequent_pattern(newp, newp->get_symbols(0)->begin(),
                                                             newp->get_symbols(0)->end());
                        } else {
                            if (ct->insert_pattern(
                                    newp))                    //NOTE this check must be after set_min_windows, because pattern equality is also based on support
                            {
                                cover = new Cover(par->seq, ct, false);
                                double new_size = cover->get_sz_sequence_and_ct();
                                int new_total_usage = cover->get_total_usage();
                                if (new_size < current_usgSz->sz)                                    //Check improvement
                                {
                                    par->cnt_acc_var++;
                                    current_usgSz = postprune(newp, new_total_usage,
                                                              new_size);        //post acceptance pruning
                                    current_usgSz = try_variations(newp,
                                                                   current_usgSz);                //recursively try variations of newp+singleton
                                } else {
                                    par->cnt_rej_var++;
                                    ct->delete_pattern(newp);
#ifdef LSH
//                                    --PatternTable::total_p_id;
#endif
                                }
                            }
#ifdef LSH
//                            else {
//                                --PatternTable::total_p_id;
//                            }
#endif
                        }
                    } else
                        delete newp;
                }
            }
        }
    }

    delete temp_cand;
    return current_usgSz;
}
#ifdef LSH
void Beep::generate_candidates(usg_sz *current_usgSz) {
    bool stop = false;
    while (!candidate_order->empty()) {
        auto top = candidate_order->top();
        if (top.first < cand_threshold * par->seq->get_nr_sequences()) {
            break;
        }
        Pattern * p1 = top.second.first;
        Pattern * p2 = top.second.second;
        if ((p1)->get_support() < par->minsup) {
            candidate_order->pop();
            continue;
        }
        if ((p2)->get_support() < par->minsup) {
            candidate_order->pop();
            continue;
        }

        auto *result = new pattern_set;
        stop = combine_patterns(p1, p2, current_usgSz->usg, result);
        if (stop)
            break;

        insert_candidates(result);
        candidate_order->pop();
    }
}
#else
void Beep::generate_candidates(usagepattern_set::iterator *pt_ct_1, usagepattern_set::iterator *pt_ct_2,
                               usagepattern_set::iterator *pt_begin_ct, usagepattern_set::iterator *pt_end_ct,
                               usg_sz *current_usgSz) {
    //order: 1x1, 1x2, 2x2, 1x3, 2x3, 3x3, 1x4, 2x4, 3x4, 4x4, 1x5, ..
    bool stop = false;
    while (*pt_ct_2 != *pt_end_ct)        //it_ct_2 is the first to reach the end
    {
        //if one of the patterns is a fill-pattern we do not combine it
        if (par->fill_patterns) {
            if ((**pt_ct_1)->get_size() == 1 &&
                par->fill_pattern[(*(**pt_ct_1)->get_symbols(0)->begin())->attribute] &&
                (*(**pt_ct_1)->get_symbols(0)->begin())->symbol ==
                par->alphabet_sizes[(*(**pt_ct_1)->get_symbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_1;
                continue;
            }
            if ((**pt_ct_2)->get_size() == 1 &&
                par->fill_pattern[(*(**pt_ct_2)->get_symbols(0)->begin())->attribute] &&
                (*(**pt_ct_2)->get_symbols(0)->begin())->symbol ==
                par->alphabet_sizes[(*(**pt_ct_2)->get_symbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_2;
                continue;
            }
        }

        //if one of the patterns support is below minsup we do not combine
        if ((**pt_ct_1)->get_support() < par->minsup) {
            ++*pt_ct_1;
            continue;
        }
        if ((**pt_ct_2)->get_support() < par->minsup) {
            ++*pt_ct_2;
            continue;
        }

        auto *result = new pattern_set;
        stop = combine_patterns(**pt_ct_1, **pt_ct_2, current_usgSz->usg, result);
        if (stop)
            break;

        insert_candidates(result);

        if (*pt_ct_1 == *pt_ct_2) {
            *pt_ct_1 = usagepattern_set::iterator(*pt_begin_ct);    //hard copy
            ++*pt_ct_2;
        } else
            ++*pt_ct_1;
    }

}
#endif

//Only adds the patterns if they are not already present
void Beep::insert_candidates(pattern_set *list) {
    auto it = list->begin(), end = list->end();
    while (it != end) {
        Pattern *p = *it;
        bool delete_pattern = false;

        if (par->prune_tree) {
            //find_pattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
            for (int startPos = 0; startPos < p->get_length(); ++startPos) {
                if (root->find_pattern(p, startPos, p->get_symbols(startPos)->begin(),
                                       p->get_symbols(startPos)->end())) {
                    par->cnt_infreq++;
                    delete_pattern = true;
                    break;        //if the pattern from one of the startPositions is found then we know enough
                }
            }
        }
        if (!delete_pattern) {
            auto candidate_end = cand->end();
            if (find_pattern_in_set(cand, p) == candidate_end)
                cand->insert(p);
            else
                delete_pattern = true;
        }
        if (delete_pattern) {

            delete p;
        }
        ++it;
    }
}


//Returns true when we do not combine x and y because of their usage compared to bestCand
//Returns a list of patterns constructed from a and b via the pattern_set* result
bool Beep::combine_patterns(Pattern *a, Pattern *b, int total_usage, pattern_set *result) {
    if (!cand->empty() && (a->get_usage() < (*cand->begin())->get_estimated_usage() ||
            b->get_usage() < (*cand->begin())->get_estimated_usage()))
        return true;

    int usg_z, usg_x, usg_y;

    //check whether they specify values for a similar attribute
    bool sim_attr = false;
    set<int> *set_x = a->get_total_aids(), *set_y = b->get_total_aids();
    auto endY = set_y->end();
    for (int it : *set_x) {
        if (set_y->find(it) != endY) {
            sim_attr = true;
            break;
        }
    }

    for (int offset = 0; offset < a->get_length() + b->get_length() + 1; ++offset) {
        if (!sim_attr || !check_pattern_attribute_overlap(a, b, offset)) {
            Pattern *newp = build_pattern(a, b, offset);
            usg_x = newp->get_x()->get_usage();
            usg_y = newp->get_y()->get_usage();
            usg_z = min(usg_x, usg_y);                        //estimated usage
            newp->set_estimated_gain(newp->compute_estimated_gain(usg_x, usg_y, usg_z, total_usage));
            result->insert(newp);
        }
    }

    return false;
}

//RETURN true when eventsets a and b contain a similar attribute
bool Beep::check_eventset_attribute_overlap(event_set *a, event_set *b) {
    bool result = false;
    bool *present_attributes = new bool[par->nr_of_attributes];
    for (int i = 0; i < par->nr_of_attributes; ++i)
        present_attributes[i] = false;

    for (auto ita : *a)  //one eventset can not contain duplicate attributes
        present_attributes[ita->attribute] = true;
    for (auto itb : *b) {
        if (present_attributes[itb->attribute]) {  //check if the attribute was already seen in 'a'
            result = true;
            break;
        }
        present_attributes[itb->attribute] = true;
    }
    delete[]present_attributes;
    return result;
}

//RETURN true when Patterns a and b overlap when combined with the given offset
bool Beep::check_pattern_attribute_overlap(Pattern *a, Pattern *b,
                                           int offset) {    //	  Offset:	0		1		2		3		4		5
    // Pattern A:   xxx		xxx		xxx		xxx		xxx		xxx
    // Pattern B: xx	   xx		xx		 xx		  xx	   xx
    //	  length:	5		4		3		3		4		5
    // start_pos_a:	2		1		0		0		0		0
    // start_pos_b:	0		0		0		1		2		3

    //	  Offset:	0		1		2		3
    // Pattern A:   x		x		x		x
    // Pattern B: xx	   xx		xx		 xx
    //	  length:	3		2		2		3
    // start_pos_a:	2		1		0		0
    // start_pos_b:	0		0		0		1

    //	  Offset:	0		1		2		3
    // Pattern A:   xx		xx		xx 		xx
    // Pattern B:  x	    x		 x		  x
    //	  length:	3		2		2		3
    // start_pos_a:	1		0		0		0
    // start_pos_b:	0		0		1		2


    if (offset == 0 || offset == a->get_length() + b->get_length())    //one pattern completely before the other
        return false;

    int start_pos_a = max(0, b->get_length() - offset), start_pos_b = max(0, offset - b->get_length());
    int new_length;
    if (start_pos_b == 0)
        new_length = max(start_pos_a + a->get_length(), b->get_length());
    else
        new_length = max(start_pos_b + b->get_length(), a->get_length());
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos >= start_pos_a && pos >= start_pos_b && pos - start_pos_a < a->get_length() &&
            pos - start_pos_b < b->get_length())
            if (check_eventset_attribute_overlap(a->get_symbols(pos - start_pos_a), b->get_symbols(pos - start_pos_b)))
                return true;
    }
    return false;
}

event_set *Beep::join_eventsets(event_set *a, event_set *b) {
    auto *result = new event_set;
    for (auto ita : *a)
        result->insert(ita);
    for (auto itb : *b)
        result->insert(itb);
    return result;
}

//construct a new pattern from a singleton and another pattern, with the singleton at the specified position
Pattern *Beep::build_interleaved_pattern(Pattern *singleton, Pattern *p, int pos_singleton) {
    int new_length = p->get_length() + 1;
    int pos_p = 0;
    auto **event_sets = new event_set *[new_length];
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos == pos_singleton)
            event_sets[pos] = new event_set(*singleton->get_symbols(0)); //hard copy
        else
            event_sets[pos] = new event_set(*p->get_symbols(pos_p++)); //hard copy
    }
    return new Pattern(new_length, event_sets, par->seq, singleton, p);
}

//construct a new pattern from a and b with given offset (already checked that there is no overlap)
Pattern *Beep::build_pattern(Pattern *a, Pattern *b, int offset) {
    Pattern *newp;

    int start_pos_a = max(0, b->get_length() - offset), start_pos_b = max(0, offset - b->get_length());
    int new_length;
    if (start_pos_b == 0)
        new_length = max(start_pos_a + a->get_length(), b->get_length());
    else
        new_length = max(start_pos_b + b->get_length(), a->get_length());

    auto **event_sets = new event_set *[new_length];
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos >= start_pos_a && pos >= start_pos_b && pos - start_pos_a < a->get_length() &&
            pos - start_pos_b < b->get_length())
            event_sets[pos] = join_eventsets(a->get_symbols(pos - start_pos_a), b->get_symbols(pos - start_pos_b));
        else if (pos >= start_pos_a && pos - start_pos_a < a->get_length())
            event_sets[pos] = new event_set(*a->get_symbols(pos - start_pos_a)); //hard copy
        else if (pos >= start_pos_b && pos - start_pos_b < b->get_length())
            event_sets[pos] = new event_set(*b->get_symbols(pos - start_pos_b)); //hard copy
    }
    newp = new Pattern(new_length, event_sets, par->seq, a, b);
    return newp;
}


usg_sz *Beep::postprune(Pattern *accepted, int total_usg, double current_size) {
    auto *pruneset = new prunepattern_set;
    for (auto p : *ct->get_ct()) {
        if (p->get_size() > 1)//singletons can't be pruned
            if (p->get_usage_decreased())
                pruneset->insert(p);
    }
    while (!pruneset->empty()) {
        //prune the top
        auto it_top = pruneset->begin();
        Pattern *top = *it_top;

        pruneset->erase(it_top);        //erase on iterator, because pruneset compares on usage
        ct->delete_pattern(top);

        cover = new Cover(par->seq, ct, false);

        double new_size = cover->get_sz_sequence_and_ct();
        if (new_size < current_size)            //Check improvement
        {
            //check if pruned pattern was subset of last accepted pattern
            if (par->prune_check) {
                if (!check_subset(top, accepted)) {
                    output_stream << "\nPruned a pattern that was not a subset of the accepted pattern.\n"
                                   << "\tAccepted: " << accepted->print(false) << "\tPruned: " << top->print(false)
                                   << endl;
                }
            }
            total_usg = cover->get_total_usage();
            current_size = new_size;
            //add more prune candidates
            for (auto p : *ct->get_ct()) {
                if (p->get_size() > 1 && p->get_usage_decreased())                //singletons can't be pruned
                    if (find_pattern_in_set(pruneset, p) != pruneset->end())    //if not already present
                        pruneset->insert(p);
            }
        } else {
            ct->rollback();
            ct->insert_pattern(top);        //put pattern back
        }
    }
    delete pruneset;
    return new usg_sz(total_usg, current_size);
}

//checks whether esa is a subset of esb
bool Beep::check_subset(event_set *esa, event_set *esb) {
    auto endb = esb->end();
    for (auto it : *esa)
        if (esb->find(it) == endb)
            return false;
    return true;
}

//checks whether a is a subset of b
bool Beep::check_subset(Pattern *a, Pattern *b) {
    int la = 0;
    for (int lb = 0; lb < b->get_length(); ++lb) {
        if (check_subset(a->get_symbols(la), b->get_symbols(lb)))
            la++;

        if (la == a->get_length())
            return true;
    }
    return false;
}


candpattern_set::iterator Beep::find_pattern_in_set(pattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

candpattern_set::iterator Beep::find_pattern_in_set(candpattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

prunepattern_set::iterator Beep::find_pattern_in_set(prunepattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

Beep::~Beep() {
    delete par->seq;
    delete ct;
    delete cand;
    delete ct_on_usg;
    delete cover;
    if (white_list)
        white_list->clear();
    delete white_list;
    delete black_list;
}
