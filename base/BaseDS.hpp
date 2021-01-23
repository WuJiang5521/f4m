//
// Created by A on 2021/1/23.
//

#ifndef FMP_BASEDS_HPP
#define FMP_BASEDS_HPP


#include <string>
#include <vector>
#include "FileLoader.hpp"

class BaseDS {
private:
    static std::list<BaseSeq> sequenceList;

public:
    static void load_file(const std::vector<std::string>& files, FileType type, const std::string& dir);
    static void convert_data(const std::string& filename);
};

#endif //FMP_BASEDS_HPP
