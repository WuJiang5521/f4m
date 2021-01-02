//
// Created by A on 2020/12/22.
//

#ifndef FMP_FMP_HPP
#define FMP_FMP_HPP

#include <list>
#include <set>
#include <iostream>

#include "Sequence/Sequence.hpp"
#include "Pattern/Pattern.hpp"

class FMP {
public:
    static std::list<Sequence> sequenceList;
    static std::set<Pattern, Pattern::codeTableSetComp> codeTable;
    static void do_fmp() {
        Pattern::totalTimeStamp = 0;
        Event e1 = Event({0, 0, 0, Pattern::patternNULLFlag});
        Event e2 = Event({1, 1, 1, 1});

        Pattern p = Pattern({e1, e2});
        codeTable.insert(p);
        Event e3 = Event({Pattern::patternNULLFlag, Pattern::patternNULLFlag, Pattern::patternNULLFlag, 2});
        Event e4 = Event({2, 2, Pattern::patternNULLFlag, Pattern::patternNULLFlag});
        Pattern p2 = Pattern({e3, e4});
        codeTable.insert(p2);

//        Sequence s = *sequenceList.begin();
//
//        s.getCover(codeTable);
//
//        for (auto & it : s.cover) {
//            for (auto it2 = it.begin(); it2 != it.end(); ++it2) {
//                std::cout << "(" << it2->first << ", " << it2->second << ")" << " ";
//            }
//            std::cout << std::endl;
//        }


        Sequence s2 = *sequenceList.rbegin();
        s2.getCover(codeTable);

        for (auto & it : s2.cover) {
            for (auto it2 = it.begin(); it2 != it.end(); ++it2) {
                std::cout << "(" << it2->first << ", " << it2->second << ")" << " ";
            }
            std::cout << std::endl;
        }

    }
};


#endif //FMP_FMP_HPP
