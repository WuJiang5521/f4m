//
// Created by A on 2020/12/22.
//

#include "Fmp.hpp"

using namespace std;

int FMP::max_seq_len = 0;
int FMP::nST = 0;
int FMP::D_total_len = 0;
vector<vector<int>> FMP::ST_support;

void FMP::do_fmp() {
    Pattern::totalTimeStamp = 0;

    max_seq_len = accumulate(sequenceList.begin(), sequenceList.end(), -1, [](int max, list<Sequence>::iterator b) {
        return max > b->size() ? max : b->size();
    });

    D_total_len = accumulate(sequenceList.begin(), sequenceList.end(), 0, [](int sum, list<Sequence>::iterator b) {
        return sum + b->size();
    });

    const std::vector<std::string> &keys = Attribute::get_keys();

    ST_support.resize(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        ST_support[i].resize(Attribute::get_attrs(keys[i]).size());
    }

    for (auto & seq : sequenceList) {
        for (auto & event : seq) {
            for (int i = 0; i < event.size(); ++i) {
                ++ST_support[i][event[i]];
            }
        }
    }

    int key_id = 0;
    for (auto &key : keys) {
        const std::vector<std::string> &attrs = Attribute::get_attrs(key);
        for (int i = 0; i < attrs.size(); ++i) {
            Event e(std::vector<int>(keys.size(), Pattern::patternNULLFlag));
            e[key_id] = i;
            auto p = new Pattern({e});
            codeTable.insert(p);
            ++nST;
        }
        ++key_id;
    }

    

}

double FMP::LCalc() {

    auto usg_sum = static_cast<double>(accumulate(codeTable.begin(), codeTable.end(), 0, [](int a, CodeTableType::iterator b) {
        return a + (*b)->getUsage();
    }));

    auto L_Cp_CT = static_cast<double>(accumulate(codeTable.begin(), codeTable.end(), 0, [usg_sum](int a, CodeTableType::iterator b) {
        int usg = (*b)->getUsage();
        return a + usg * (-log(usg / usg_sum));
    }));

    auto L_Cg_CT = static_cast<double>(accumulate(codeTable.begin(), codeTable.end(), 0, [usg_sum](int a, CodeTableType::iterator b) {
        if ((*b)->size() > 1) {
            int gaps = (*b)->getGaps();
            int fills = (*b)->getFills();
            return a + (gaps * (-log(gaps / (gaps + fills))) + fills * (-log(fills / (gaps + fills))));
        } else {
            return static_cast<double>(a);
        }
    }));

    double L_D_CT = L_Cp_CT + L_Cg_CT + MathUtil::intcost(Attribute::get_keys().size())
            + MathUtil::intcost(sequenceList.size())
            + accumulate(sequenceList.begin(), sequenceList.end(), 0.0, [](double a, list<Sequence>::iterator b) {
                return a + MathUtil::intcost(b->size());
            });

    auto keys = Attribute::get_keys();
    int P = codeTable.size() - nST;
    int usageP = accumulate(codeTable.begin(), codeTable.end(), 0, [](int a, CodeTableType::iterator b) {
        if ((*b)->size() > 1) {
            return a + (*b)->getUsage();
        } else {
            return a;
        }
    });
    double L_CT = accumulate(keys.begin(), keys.end(), 0.0, [](double a, vector<string>::iterator b) {
                int Omega = Attribute::get_attrs(*b).size();
                return a + MathUtil::intcost(Omega) + log(MathUtil::lg_choose(max_seq_len, Omega));
            }) + MathUtil::intcost(P + 1) + MathUtil::intcost(usageP + 1) + log(MathUtil::lg_choose(usageP, P))
            + accumulate(codeTable.begin(), codeTable.end(), 0.0, [](double sum, CodeTableType::iterator it){
                if ((*it)->size() != 1 && (*it)->getUsage() != 0) {
                    return sum +  MathUtil::intcost((*it)->size()) + MathUtil::intcost((*it)->getGaps() + 1)
                           + (*it)->size() * log(Attribute::get_keys().size())
                           + accumulate((*it)->begin(), (*it)->end(), 0.0, [](double sum, vector<Event>::iterator it) {
                               int support_x = 0;
                               for (int i = 0; i < it->size(); ++i) {
                                   support_x += ST_support[i][(*it)[i]];
                               }
                                return -log(support_x / D_total_len);
                           });
                } else {
                    return sum;
                }
            });

    return L_D_CT + L_CT;
}