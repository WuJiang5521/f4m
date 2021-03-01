//
// Created by A on 2021/1/23.
//

#ifndef LSH_BASESEQ_HPP
#define LSH_BASESEQ_HPP


#include "BaseEvt.hpp"

class BaseSeq {
public:
    explicit BaseSeq(std::vector<BaseEvt> );
    std::vector<BaseEvt>::iterator begin();
    std::vector<BaseEvt>::iterator end();
    friend std::ostream & operator <<(std::ostream &out, const BaseSeq &sequence);
    [[nodiscard]] int size() const;
private:
    std::vector<BaseEvt> events;
};


#endif //LSH_BASESEQ_HPP
