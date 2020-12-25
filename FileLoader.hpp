//
// Created by A on 2020/12/22.
//

#ifndef FMP_FILELOADER_HPP
#define FMP_FILELOADER_HPP

#include <list>
#include <fstream>
#include "Sequence.hpp"

enum FileType {
    TableTennis,
};

class FileLoader {
public:
    static std::list<Sequence> loadFile(const std::string& fileName, FileType fileType);
};

#endif //FMP_FILELOADER_HPP
