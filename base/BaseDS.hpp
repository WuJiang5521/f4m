//
// Created by A on 2021/1/23.
//

#ifndef LSH_BASEDS_HPP
#define LSH_BASEDS_HPP


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

#endif //LSH_BASEDS_HPP
