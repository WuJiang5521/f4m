//
// Created by A on 2020/12/22.
//

#ifndef LSH_FILELOADER_HPP
#define LSH_FILELOADER_HPP

#include <list>
#include <fstream>
#include "BaseSeq.hpp"

enum FileType {
    TableTennis,
};

class FileLoader {
public:
    static std::list<BaseSeq> loadFile(const std::string& fileName, FileType fileType);
};

#endif //LSH_FILELOADER_HPP
