//
// Created by A on 2020/12/22.
//

#ifndef FMP_BASEATTRIBUTE_HPP
#define FMP_BASEATTRIBUTE_HPP


#include <string>
#include <map>
#include <vector>

class BaseAttribute {
public:
    static int from_key_value(const std::string &key, const std::string &value);
    static std::string get_key_value(const std::string &key, int attr);

    static std::string get_record();
    static const std::vector<std::string> & get_keys();
    static const std::vector<std::string> & get_attrs(const std::string &);

private:
    static std::vector<std::string> keys;
    static std::map<std::string, std::map<std::string, int>> mapping;
    static std::map<std::string, std::vector<std::string>> record;
};


#endif //FMP_BASEATTRIBUTE_HPP
