//
// Created by A on 2020/12/22.
//

#include <iostream>
#include "FileLoader.hpp"
#include "json.hpp"
#include <exception>

using namespace std;
using json = nlohmann::json;

list<Sequence> extract_tt_sequences(json j) {
    try {
        list<Sequence> sequenceList = list<Sequence>();

        for (auto game: j["record"]["list"]) {
            for (auto rally: game["list"]) {
                auto events = vector<Event>();
                for (auto strike: rally["list"]) {
                    auto attrs = vector<int>();

                    attrs.push_back(Attribute::from_key_value("ttBallPosition", strike["BallPosition"]));
                    attrs.push_back(Attribute::from_key_value("ttStrikePosition", strike["StrikePosition"]));
                    attrs.push_back(Attribute::from_key_value("ttStrikeTech", strike["StrikeTech"]));
                    attrs.push_back(Attribute::from_key_value("ttSpinKind", strike["SpinKind"]));

                    events.emplace_back(attrs);
                }
                sequenceList.emplace_back(events);
            }
        }

        return sequenceList;
    } catch (exception &msg) {
        throw invalid_argument("Unrecognizable file format!");
    }
}

list<Sequence> FileLoader::loadFile(const string& fileName, FileType fileType) {
    try {
        ifstream file(fileName, ios::in);
        if (!file.is_open())
            throw invalid_argument("Cannot open this file!");


        json j;
        file >> j;

        switch (fileType) {
            case FileType::TableTennis:
                return extract_tt_sequences(j);
            default:
                throw invalid_argument("The file type is not supported!");
        }
    } catch (exception &msg) {
        cout << "Error: " << msg.what() << endl;
        return list<Sequence>();
    }
}
