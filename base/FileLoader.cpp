//
// Created by A on 2020/12/22.
//

#include <iostream>
#include "FileLoader.hpp"
#include "BaseAttr.hpp"
#include "json.hpp"
#include <exception>
//#define FLITER_SERVER
using namespace std;
using json = nlohmann::json;

list<BaseSeq> extract_tt_sequences(json j) {
    try {
        list<BaseSeq> sequenceList = list<BaseSeq>();

        for (auto game: j["record"]["list"]) {
            for (auto rally: game["list"]) {
                auto events = vector<BaseEvt>();
#ifdef FLITER_SERVER
                if (rally["list"].size() <= 3) {
                    continue;
                }
                if (rally["list"][0]["HitPlayer"] == "00") {
                    continue;
                }
#endif
                for (auto strike: rally["list"]) {
                    auto attrs = vector<int>();

                    attrs.push_back(BaseAttr::from_key_value("ttBallPosition", strike["BallPosition"]));
                    attrs.push_back(BaseAttr::from_key_value("ttStrikePosition", strike["StrikePosition"]));
                    attrs.push_back(BaseAttr::from_key_value("ttStrikeTech", strike["StrikeTech"]));
                    attrs.push_back(BaseAttr::from_key_value("ttSpinKind", strike["SpinKind"]));

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

list<BaseSeq> FileLoader::loadFile(const string& fileName, FileType fileType) {
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
        return list<BaseSeq>();
    }
}