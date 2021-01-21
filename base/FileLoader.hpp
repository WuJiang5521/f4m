//
// Created by A on 2020/12/22.
//

#ifndef FMP_FILELOADER_HPP
#define FMP_FILELOADER_HPP

#include <list>
#include <fstream>
#include "../FMP/Sequence/FMPSequence.hpp"

enum FileType {
    TableTennis,
};

class FileLoader {
public:
    static std::list<FMPSequence> loadFile(const std::string& fileName, FileType fileType);
};

#endif //FMP_FILELOADER_HPP
