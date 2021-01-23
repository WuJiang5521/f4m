//
// Created by A on 2021/1/23.
//

#include "BaseDS.hpp"
#include "BaseAttr.hpp"
#include <iostream>
#include <ctime>

using namespace std;

std::list<BaseSeq> BaseDS::sequenceList = {};

void BaseDS::load_file(const vector<string> &files, FileType type, const string &dir = "") {
    clock_t t1 = clock();

    for (const auto &file: files)
        sequenceList.splice(sequenceList.end(), FileLoader::loadFile(dir + file, type));

    cout << "Finish Loading! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
    cout << "Attribute mapping:" << endl << BaseAttr::get_record() << endl;
    cout << "Total sequences: " << sequenceList.size() << endl;
}

void BaseDS::convert_data(const string &filename) {
    ofstream f;
    f.open(filename, ios::out);
    auto attrs = BaseAttr::get_keys();
    int nrAttr = attrs.size();
    f << nrAttr;
    for (const auto &attr: attrs)
        f << " " << BaseAttr::get_attrs(attr).size();
    for (int i = 0; i < nrAttr; i++) {
        f << endl;
        for (auto sequence = sequenceList.begin(); sequence != sequenceList.end(); sequence++) {
            if (sequence != sequenceList.begin()) f << -1 << " ";
            for (auto &event : (*sequence)) {
                f << event[i] << " ";
            }
        }
        if (i != nrAttr - 1) f << -2;
    }
    f.close();
}
