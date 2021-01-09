//
// Created by A on 2020/12/22.
//

#include "Fmp.hpp"

using namespace std;

int FMP::max_seq_len = 0;
int FMP::nST = 0;
int FMP::D_total_len = 0;
vector<vector<int>> FMP::ST_support;
Pattern * FMP::nullPattern = nullptr;

void FMP::do_fmp() {
    Pattern::totalTimeStamp = 0;

    max_seq_len = accumulate(sequenceList.begin(), sequenceList.end(), -1, [](int max, Sequence b) {
        return max > b.size() ? max : b.size();
    });

    D_total_len = accumulate(sequenceList.begin(), sequenceList.end(), 0, [](int sum, Sequence b) {
        return sum + b.size();
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
    nullPattern = new Pattern({Event(vector<int>(keys.size(), Pattern::patternNULLFlag))});
    codeTable.insert(nullPattern);
    for (auto& p : codeTable) {
        P_PTable::table[p] = map<Pattern*, int>();
        for (auto& p1 : codeTable) {
            P_PTable::table[p][p1] = 0;
        }
    }
    CodeTableType candTable = codeTable;
    for (auto & c : candTable) {
        cout << c << ": ";
        for (auto & e : c->pattern) {
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


    list<Sequence> candidateSequenceList(sequenceList);
    bool is_updated;
    do {
        cout << curL << endl;
        is_updated = false;
        for (auto Xit = codeTable.begin(); Xit != codeTable.end(); ++Xit) {
            auto X = *Xit;
            vector<Pattern *> cands = getCand(X);
            for (auto & cand : cands) {
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
        for (auto & c : candTable) {
            for (auto & e : *c) {
                cout << e << " ";
            } cout << endl;
            if (codeTable.find(c) == codeTable.end()) {
                codeTable.insert(c);
            }
        }
    } while (is_updated);
    cout << "L time: " << t_all << endl;
    cout << "cover time: " << t_cover << endl;

}

bool isSingleton(Pattern & X) {
    if (X.size() > 1) {
        return false;
    }
    int cnt = 0;
    for (int i  = 0; i < X[0].size(); ++i) {
        cnt += (int)(X[0][i] == Pattern::patternNULLFlag);
    }
    return cnt == (Attribute::get_keys().size() - 1);
}

void FMP::prune(std::list<Sequence> & sl, CodeTableType & table, double & curL) {
    vector<CodeTableType::iterator> pruneCands;
    for (auto Xit = table.begin(); Xit != table.end(); ++Xit) {
        auto X = *Xit;
        if (X->oldUsg > X->usg) {
            pruneCands.push_back(Xit);
        }
    }
    std::list<Sequence> tmpSL(sl);
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

Pattern * FMP::getAlignment(Pattern * X, Pattern * P, int pos) {
    if (pos < 0) {
        return getAlignment(P, X, -pos);
    }
    auto * align = new Pattern(X->pattern);
    if (align == X) {
        cout << "align duplicated!" << endl;
    }
    bool legal = true;
    int pos_P = 0;
    while (pos < X->size() && pos_P < P->size()) {
        for (int i  = 0; i < (*align)[pos].size(); ++i) {
            if ((*P)[pos_P][i] == Pattern::patternNULLFlag) {
                continue;
            }
            if ((*align)[pos][i] == Pattern::patternNULLFlag) {
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
        delete(align);
        return nullptr;
    }
    while (pos_P < P->size()) {
        align->push_back((*P)[pos_P]);
        ++pos_P;
    }
    return align;
}

vector<Pattern *> FMP::getCand(Pattern * X) {
    Pattern * maxP = X;
    int maxx = -1;
    for (auto & p : P_PTable::table[X]) {
        if (maxx < p.second) {
            maxP = p.first;
            maxx = p.second;
        }
    }
    vector<Pattern *> cands;
    int posP = X->size();
    while (posP >= -maxP->size()) {
        Pattern * cand = getAlignment(X, maxP, posP);
        if (cand != nullptr) {
            cands.push_back(cand);
        }
        --posP;
    }
    P_PTable::table[X].erase(maxP);
    P_PTable::table[maxP].erase(X);
    return cands;
}

void FMP::coverSequenceList(list<Sequence> & sl, CodeTableType &table) {
    for (auto & seq : sl) {
        seq.getCover(table);
    }
}

void FMP::checkCodeTable(CodeTableType & table) {
    for (auto & p : table) {
        p->checkup();
    }
}
void FMP::clearCodeTable(CodeTableType & table) {
    for (auto & p : table) {
        p->clearCnts();
    }
}
void FMP::rollbackCodeTable(CodeTableType & table) {
    for (auto & p : table) {
        p->rollback();
    }
}

double FMP::LCalc(CodeTableType &table) {
    if (nullPattern->usg > 0) {
        return 1e100;
    }

    auto usg_sum = static_cast<double>(accumulate(table.begin(), table.end(), 0, [](int a, Pattern* b) {
        if (b->disabled) {
            return a;
        }
        return a + b->getUsage();
    }));

    auto L_Cp_CT = static_cast<double>(accumulate(table.begin(), table.end(), 0, [usg_sum](int a, Pattern* b) {
        if (b->disabled) {
            return static_cast<double>(a);
        }
        int usg = b->getUsage();
        if (usg == 0) {
            return static_cast<double>(a);
        }
        return a + usg * (-log((double)usg / (double)usg_sum));
    }));


    auto L_Cg_CT = static_cast<double>(accumulate(table.begin(), table.end(), 0, [](int a, Pattern* b) {
        if (b->disabled) {
            return static_cast<double>(a);
        }
        if (b->size() > 1) {
            int gaps = b->getGaps();
            int fills = b->getFills();
            return a
                    + ( (gaps > 0 ? gaps * (-log((double)gaps / (double)(gaps + fills))) : 0)
                    + (fills > 0 ? fills * (-log((double)fills / (double)(gaps + fills))) : 0));
        } else {
            return static_cast<double>(a);
        }
    }));

    double L_D_CT = L_Cp_CT + L_Cg_CT + MathUtil::intcost(Attribute::get_keys().size())
            + MathUtil::intcost(sequenceList.size())
            + accumulate(sequenceList.begin(), sequenceList.end(), 0.0, [](double a, const Sequence& b) {
                return a + MathUtil::intcost(b.size());
            });

    auto keys = Attribute::get_keys();
    int P = table.size() - nST;
    int usageP = accumulate(table.begin(), table.end(), 0, [](int a, Pattern* b) {
        if (b->disabled) {
            return a;
        }
        if (b->size() > 1) {
            return a + b->getUsage();
        } else {
            return a;
        }
    });
    double L_CT = accumulate(keys.begin(), keys.end(), 0.0, [](double a, const string& b) {
                int Omega = Attribute::get_attrs(b).size();
                return a + MathUtil::intcost(Omega) + MathUtil::lg_choose(max_seq_len, Omega);
            }) + MathUtil::intcost(P + 1) + MathUtil::intcost(usageP + 1) + MathUtil::lg_choose(usageP, P)
            + accumulate(table.begin(), table.end(), 0.0, [](double sum, Pattern* it){
                if (it->disabled) {
                    return sum;
                }
                if (it->size() != 1 && it->getUsage() != 0) {
                    return sum +  MathUtil::intcost(it->size()) + MathUtil::intcost(it->getGaps() + 1)
                           + it->size() * log(Attribute::get_keys().size())
                           + accumulate(it->begin(), it->end(), 0.0, [](double sum, Event it) {
                               int support_x = 0;
                               for (int i = 0; i < it.size(); ++i) {
                                   support_x += ST_support[i][it[i]];
                               }
                                return -log((double)support_x / (double)D_total_len);
                           });
                } else {
                    return sum;
                }
            });


    return L_D_CT + L_CT;
}