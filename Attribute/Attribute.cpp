//
// Created by A on 2020/12/22.
//

#include <algorithm>
#include <sstream>
#include <iostream>
#include "Attribute.hpp"

using namespace std;


map<string, vector<string>> Attribute::record = map<string, vector<string>>();
map<string, map<string, int>> Attribute::mapping = map<string, map<string, int>>();
vector<string> Attribute::keys = vector<string>();

int Attribute::from_key_value(const string &key, const string &value) {
    if (record.count(key) == 0) {
        keys.push_back(key);
        record.insert(pair<string, vector<string>>(key, vector<string>()));
        mapping.insert(pair<string, map<string, int>>(key, map<string, int>()));
    }

    auto &v = record[key];
    auto &m = mapping[key];
    if (m.count(value) == 0) {
        int pos = v.size();
        v.push_back(value);
        m.insert(pair<string, int>(value, pos));
        return pos;
    } else {
        return m[value];
    }
}

const vector<string> & Attribute::get_keys() {
    return keys;
}

const vector<string> & Attribute::get_attrs(const string & key) {
    return record[key];
}


string Attribute::get_key_value(const string &key, const int attr) {
    if (record.count(key) == 0)
        return to_string(attr);

    auto v = record[key];
    if (attr >= 0 && attr < v.size()) return v[attr];
    else return to_string(attr);
}

string Attribute::get_record() {
    stringstream ss;
    for (const auto &rec: record) {
        ss << rec.first << "\t\t: ";
        for (int i = 0; i < rec.second.size(); i++) {
            if (i != 0) ss << ", ";
            ss << "(" << i << "," << rec.second.at(i) << ")";
        }
        ss << endl;
    }
    return ss.str();
}
