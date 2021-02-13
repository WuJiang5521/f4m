#include "Common.h"
#include "Cover.h"
/*	Changes the usage of the Patterns in the CodeTable as a result of the covering process
	bool other_data: false = loop through the block's occurences, true = try to fit block on all positions in data */
Cover::Cover(Sequence *s, CodeTable *codeTable, bool other_data) : sequence(s),
                                                                   code_table(codeTable),
                                                                   other_data(other_data) {
    sequence->get_parameters()->cnt_covers++;
#ifdef LSH

    delete[] sequence->cover_pattern;

    sequence->cover_pattern = new set<Pattern*>[sequence->get_nr_sequences()];
#endif

    total_usage = 0;
    sz_sequence_and_ct = 0;
    sequence->reset_cover();

    bool cover_complete = false;
    codeTable_set *ct = codeTable->get_ct();
    Pattern *candidate;

    par = sequence->get_parameters();

    //make an array to quickly get pointers to singletons based on their attribute and symbol
    int nr_attr = par->nr_of_attributes;
    int *alp_szs = par->alphabet_sizes;

    auto ***singletons = new Pattern **[nr_attr];
    for (int atr = 0; atr < nr_attr; ++atr)
        singletons[atr] = new Pattern *[alp_szs[atr]];

    //Standard Cover Order
    for (auto it_blockList : *ct) {
        candidate = it_blockList;

        if (candidate->get_size() == 1) {
            //optimisation -> when arrived at singletons just loop once over data and cover all uncovered symbols with singletons
            cover_complete = true;                                        //to make sure the usages of all singletons are reset
            singletons[(*candidate->get_symbols(0)->begin())->attribute][(*candidate->get_symbols(
                    0)->begin())->symbol] = candidate;
        }

        candidate->reset_usage();                                        //reset the usage for the Pattern in the code_table before covering
        if (cover_complete)
            continue;                                                    //do not break, because we still need to reset all usages

        cover_complete = cover_with_pattern_min_windows(candidate);
    }

    //cover the rest with singletons
    sequence->cover_singletons(singletons);
#ifdef LSH
    for (int seq_id = 0; seq_id < sequence->get_nr_sequences(); ++seq_id) {
        for (auto cp1 = sequence->cover_pattern[seq_id].begin(); cp1 != sequence->cover_pattern[seq_id].end(); ++cp1) {
            for (auto cp2 = cp1; cp2 != sequence->cover_pattern[seq_id].end(); ++cp2) {
                (*PatternTable::table)[PatternTable::pattern_id_map[*cp1]][PatternTable::pattern_id_map[*cp2]]++;
            }
        }
    }
#endif
    for (int atr = 0; atr < nr_attr; ++atr)
        delete[]singletons[atr];
    delete[]singletons;

    compute_total_usage(ct);                                                //excluding laplace
#ifdef MISS
    compute_total_usage_miss(ct);
#endif
    update_pattern_codelengths(
            ct);                                        //only changes CL when we other_data = false
    sz_sequence_and_ct = codeTable->compute_sz(sequence);
}

void Cover::compute_total_usage(codeTable_set *ct) {
    total_usage = 0;
    for (auto it_blockList : *ct)
        total_usage += it_blockList->get_usage();
}

#ifdef MISS
void Cover::compute_total_usage_miss(codeTable_set *ct) {
    total_usage_miss = 0;
    for (auto it_blockList : *ct)
        total_usage_miss += it_blockList->get_usage_miss();
}
#endif

//cover all minimal windows of the pattern (when not covered yet)
//RETURN true if the cover is complete afterwards
bool Cover::cover_with_pattern_min_windows(Pattern *p) {
    bool cover_complete = false;

#ifdef MISS
//    if (miss_print_debug) {
//        outfile_miss << "pattern: " << p->print() << endl;
//    }
#endif
    //loop through all this block's minimal windows
    list<Window *> *lst = p->get_min_windows(sequence, other_data);

    for (auto w : *lst) { //defines a minimal window
#ifdef MISS
        int miss_cnt;
        if ((miss_cnt = cover_window_with_pattern(w, p)) != -1) {
#else
        if (cover_window_with_pattern(w, p)) {
#endif
            //cover this window with pattern p (NOTE: we do this only AFTER having checked try_cover
            //TODO FUTURE: try if alternative of other patterns is cheaper
            w->active = true;
#ifdef MISS
            p->update_usages(w->get_gap_length(), miss_cnt);
#else
            p->update_usages(w->get_gap_length());
#endif
            cover_complete = sequence->cover(p, w);
#ifdef LSH
            int seq_id = w->get_mev_position(0)->seqid;
            sequence->cover_pattern[seq_id].insert(p);
#endif
        }

        if (cover_complete)
            break;
    }
    return cover_complete;
}
#ifdef MISS
int Cover::cover_window_with_pattern(Window *w, Pattern *p) {
#else
bool Cover::cover_window_with_pattern(Window *w, Pattern *p) {
#endif
    //check if gap is not bigger than patternlength-1
    if (w->get_gap_length() > p->get_length() - 1)
#ifdef MISS
        return -1;
#else
        return false;
#endif
#ifdef MISS
    int miss_cnt = 0;
#endif
    //for each timestep in the pattern test whether the data can still be covered for this window
    for (int ts = 0; ts < p->get_length(); ++ts) {
#ifdef MISS
//        if (miss_print_debug) {
//            outfile_miss << "pattern position: " << ts << endl;
//        }
        miss_cnt += sequence->try_cover(p->get_symbols(ts), w->get_mev_position(ts)->id);
        if (miss_cnt > (p->get_size() + 5) / 10) {
            return -1;
        }
#else
        if (!sequence->try_cover(p->get_symbols(ts), w->get_mev_position(ts)->id))
            return false;
#endif
    }
#ifdef MISS
    return miss_cnt;
#else
    return true;
#endif
}


void Cover::update_pattern_codelengths(codeTable_set *ct) {
    if (!other_data) {
        double sum = (double) total_usage + code_table->get_ct_length() * laplace;        //including laplace
#ifdef MISS
        double miss_sum = (double) total_usage_miss;
#endif
        //loop through all Patterns in the code_table to update their optimal codelength

        for (auto it_block_list : *ct)
#ifdef MISS
            it_block_list->update_codelength(sum, miss_sum, code_table->get_math_util(), par->nr_of_attributes);
#else
            it_block_list->update_codelength(sum);
#endif
    }
}


Cover::~Cover() = default;
