//
// Created by A on 2020/12/22.
//

#include <ctime>
#include "Fmp.hpp"

using namespace std;

int FMP::max_seq_len = 0;
int FMP::nST = 0;
int FMP::D_total_len = 0;
vector<vector<int>> FMP::ST_support;
FmpPattern *FMP::nullPattern = nullptr;

void FMP::do_fmp() {
    FmpPattern::totalTimeStamp = 0;

    max_seq_len = accumulate(sequenceList.begin(), sequenceList.end(), -1, [](int max, FMPSequence b) {
        return max > b.size() ? max : b.size();
    });

    D_total_len = accumulate(sequenceList.begin(), sequenceList.end(), 0, [](int sum, FMPSequence b) {
        return sum + b.size();
    });

    const std::vector<std::string> &keys = FMPAttribute::get_keys();

    ST_support.resize(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        ST_support[i].resize(FMPAttribute::get_attrs(keys[i]).size());
    }

    for (auto &seq : sequenceList) {
        for (auto &event : seq) {
            for (int i = 0; i < event.size(); ++i) {
                ++ST_support[i][event[i]];
            }
        }
    }

    int key_id = 0;
    for (auto &key : keys) {
        const std::vector<std::string> &attrs = FMPAttribute::get_attrs(key);
        for (int i = 0; i < attrs.size(); ++i) {
            FMPEvent e(std::vector<int>(keys.size(), FmpPattern::patternNULLFlag));
            e[key_id] = i;
            auto p = new FmpPattern({e});
            codeTable.insert(p);
            ++nST;
        }
        ++key_id;
    }
    nullPattern = new FmpPattern({FMPEvent(vector<int>(keys.size(), FmpPattern::patternNULLFlag))});
    codeTable.insert(nullPattern);
    for (auto &p : codeTable) {
        P_PTable::table[p] = map<FmpPattern *, int>();
        for (auto &p1 : codeTable) {
            P_PTable::table[p][p1] = 0;
        }
    }
    CodeTableType candTable = codeTable;
    for (auto &c : candTable) {
        cout << c << ": ";
        for (auto &e : c->pattern) {
            cout << e << " ";
        }
        cout << endl;
    }
    coverSequenceList(sequenceList, candTable);
//    sequenceList.rbegin()->getCover(candTable);
    double curL = LCalc(candTable);

    auto it = sequenceList.rbegin();
//    for (auto & e : it->cover) {
//        cout << "(";
//        for (auto & a : e) {
//            cout << a.first << "," << a.second << " ";
//        } cout << ") ";
//    } cout << endl;

    clock_t t_all = 0, t_cover = 0;


    list<FMPSequence> candidateSequenceList(sequenceList);
    bool is_updated;
    do {
        cout << curL << endl;
        is_updated = false;
        for (auto Xit = codeTable.begin(); Xit != codeTable.end(); ++Xit) {
            auto X = *Xit;
            vector<FmpPattern *> cands = getCand(X);
            for (auto &cand : cands) {
                checkCodeTable(candTable);
                candTable.insert(cand);
                P_PTable::checkTable();
                P_PTable::reserveForPattern(cand);

                clearCodeTable(candTable);
                P_PTable::clearTable();
                clock_t t0 = clock();
                coverSequenceList(candidateSequenceList, candTable);
//                candidateSequenceList.rbegin()->getCover(candTable);

//                cout << "Cand: " << cand << ":: ";
//                for (auto & e : (*cand)) {
//                    cout << e << " ";
//                } cout << endl;
//
//                auto debug_it = candidateSequenceList.rbegin();
//                for (auto & e : debug_it->cover) {
//                    cout << "(";
//                    for (auto & a : e) {
//                        cout << a.first << "," << a.second << " ";
//                    } cout << ") ";
//                } cout << endl;
                clock_t t1 = clock();
                double candL = LCalc(candTable);

                t_all += clock() - t1;
                t_cover += t1 - t0;

                cout << candL << endl;
                if (candL < curL) {
                    cout << "YES!";
                    cout << endl;
                    sequenceList = candidateSequenceList;
                    curL = candL;
                    is_updated = true;

                } else {
                    candTable.erase(candTable.find(cand));
                    P_PTable::eraseForPattern(cand);
                    rollbackCodeTable(candTable);
                    P_PTable::rollbackTable();
                    delete (cand);
                }
            }
        }
        for (auto &c : candTable) {
            for (auto &e : *c) {
                cout << e << " ";
            }
            cout << endl;
            if (codeTable.find(c) == codeTable.end()) {
                codeTable.insert(c);
            }
        }
    } while (is_updated);
    cout << "L time: " << t_all << endl;
    cout << "cover time: " << t_cover << endl;

}

bool isSingleton(FmpPattern &X) {
    if (X.size() > 1) {
        return false;
    }
    int cnt = 0;
    for (int i = 0; i < X[0].size(); ++i) {
        cnt += (int) (X[0][i] == FmpPattern::patternNULLFlag);
    }
    return cnt == (FMPAttribute::get_keys().size() - 1);
}

void FMP::prune(std::list<FMPSequence> &sl, CodeTableType &table, double &curL) {
    vector<CodeTableType::iterator> pruneCands;
    for (auto Xit = table.begin(); Xit != table.end(); ++Xit) {
        auto X = *Xit;
        if (X->oldUsg > X->usg) {
            pruneCands.push_back(Xit);
        }
    }
    std::list<FMPSequence> tmpSL(sl);
    for (auto it = pruneCands.begin(); it != pruneCands.end(); ++it) {
        if (**it == nullPattern) {
            continue;
        }
        if (table.find(**it) == table.end()) {
            continue;
        }
        (**it)->disabled = true;
        checkCodeTable(table);
        P_PTable::checkTable();

        clearCodeTable(table);
        P_PTable::clearTable();
        coverSequenceList(tmpSL, table);
        double prunedL = LCalc(table);
        if (prunedL < curL) {
            if (isSingleton(***it)) {
                nST--;
            }
            P_PTable::eraseForPattern(**it);
            table.erase(*it);
            curL = prunedL;
            sl = tmpSL;
            delete (**it);
            for (auto Xit = table.begin(); Xit != table.end(); ++Xit) {
                auto X = *Xit;
                if (X->oldUsg > X->usg) {
                    pruneCands.push_back(Xit);
                }
            }
        } else {
            (**it)->disabled = false;
            rollbackCodeTable(table);
            P_PTable::rollbackTable();
        }
    }
}

FmpPattern *FMP::getAlignment(FmpPattern *X, FmpPattern *P, int pos) {
    if (pos < 0) {
        return getAlignment(P, X, -pos);
    }
    auto *align = new FmpPattern(X->pattern);
    if (align == X) {
        cout << "align duplicated!" << endl;
    }
    bool legal = true;
    int pos_P = 0;
    while (pos < X->size() && pos_P < P->size()) {
        for (int i = 0; i < (*align)[pos].size(); ++i) {
            if ((*P)[pos_P][i] == FmpPattern::patternNULLFlag) {
                continue;
            }
            if ((*align)[pos][i] == FmpPattern::patternNULLFlag) {
                (*align)[pos][i] = (*P)[pos_P][i];
            } else {
                legal = false;
                break;
            }
        }
        ++pos;
        ++pos_P;
    }
    if (!legal) {
        delete (align);
        return nullptr;
    }
    while (pos_P < P->size()) {
        align->push_back((*P)[pos_P]);
        ++pos_P;
    }
    return align;
}

vector<FmpPattern *> FMP::getCand(FmpPattern *X) {
    FmpPattern *maxP = X;
    int maxx = -1;
    for (auto &p : P_PTable::table[X]) {
        if (maxx < p.second) {
            maxP = p.first;
            maxx = p.second;
        }
    }
    vector<FmpPattern *> cands;
    int posP = X->size();
    while (posP >= -maxP->size()) {
        FmpPattern *cand = getAlignment(X, maxP, posP);
        if (cand != nullptr) {
            cands.push_back(cand);
        }
        --posP;
    }
    P_PTable::table[X].erase(maxP);
    P_PTable::table[maxP].erase(X);
    return cands;
}

void FMP::coverSequenceList(list<FMPSequence> &sl, CodeTableType &table) {
    for (auto &seq : sl) {
        seq.getCover(table);
    }
}

void FMP::checkCodeTable(CodeTableType &table) {
    for (auto &p : table) {
        p->checkup();
    }
}

void FMP::clearCodeTable(CodeTableType &table) {
    for (auto &p : table) {
        p->clearCnts();
    }
}

void FMP::rollbackCodeTable(CodeTableType &table) {
    for (auto &p : table) {
        p->rollback();
    }
}

double FMP::LCalc(CodeTableType &table) {
    if (nullPattern->usg > 0) {
        return 1e100;
    }

    auto usg_sum = static_cast<double>(accumulate(table.begin(), table.end(), 0, [](int a, FmpPattern *b) {
        if (b->disabled) {
            return a;
        }
        return a + b->getUsage();
    }));

    auto L_Cp_CT = static_cast<double>(accumulate(table.begin(), table.end(), 0, [usg_sum](int a, FmpPattern *b) {
        if (b->disabled) {
            return static_cast<double>(a);
        }
        int usg = b->getUsage();
        if (usg == 0) {
            return static_cast<double>(a);
        }
        return a + usg * (-log((double) usg / (double) usg_sum));
    }));


    auto L_Cg_CT = static_cast<double>(accumulate(table.begin(), table.end(), 0, [](int a, FmpPattern *b) {
        if (b->disabled) {
            return static_cast<double>(a);
        }
        if (b->size() > 1) {
            int gaps = b->getGaps();
            int fills = b->getFills();
            return a
                   + ((gaps > 0 ? gaps * (-log((double) gaps / (double) (gaps + fills))) : 0)
                      + (fills > 0 ? fills * (-log((double) fills / (double) (gaps + fills))) : 0));
        } else {
            return static_cast<double>(a);
        }
    }));

    double L_D_CT = L_Cp_CT + L_Cg_CT + MathUtil::intcost(FMPAttribute::get_keys().size())
                    + MathUtil::intcost(sequenceList.size())
                    + accumulate(sequenceList.begin(), sequenceList.end(), 0.0, [](double a, const FMPSequence &b) {
        return a + MathUtil::intcost(b.size());
    });

    auto keys = FMPAttribute::get_keys();
    int P = table.size() - nST;
    int usageP = accumulate(table.begin(), table.end(), 0, [](int a, FmpPattern *b) {
        if (b->disabled) {
            return a;
        }
        if (b->size() > 1) {
            return a + b->getUsage();
        } else {
            return a;
        }
    });
    double L_key = accumulate(keys.begin(), keys.end(), 0.0, [](double a, const string &b) {
        int Omega = FMPAttribute::get_attrs(b).size();
        return a + MathUtil::intcost(Omega) + MathUtil::lg_choose(max_seq_len, Omega);
    });
    double L_P = MathUtil::intcost(P + 1);
    double L_usageP = MathUtil::intcost(usageP + 1);
    double L_lg_choose = MathUtil::lg_choose(usageP, P);
    double L_table = accumulate(table.begin(), table.end(), 0.0, [](double sum, FmpPattern *it) {
        if (it->disabled) {
            return sum;
        }
        if (it->size() != 1 && it->getUsage() != 0) {
            double c1 = MathUtil::intcost(it->size());
            double c2 = MathUtil::intcost(it->getGaps() + 1);
            double c3 = it->size() * log(FMPAttribute::get_keys().size());
            double c4 = accumulate(it->begin(), it->end(), 0.0, [](double sum, FMPEvent it) {
                int support_x = 0;
                for (int i = 0; i < it.size(); ++i) {
                    if (it[i] < 0 || it[i] >= ST_support[i].size()) continue;
                    support_x += ST_support[i][it[i]];
                }
                return -log((double) support_x / (double) D_total_len);
            });
            return sum + c1 + c2 + c3 + c4;
        } else {
            return sum;
        }
    });
    double L_CT = L_key + L_P + L_usageP + L_lg_choose + L_table;


    return L_D_CT + L_CT;
}