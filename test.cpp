#include <iostream>
#include "BEEP/Beep.h"
#include "base/BaseDS.hpp"

using namespace std;

int main() {
    string dir = "../data/tt/";
    vector<string> files = {
//            "20180324 德国公开赛 女单第二轮 孙颖莎vs伊藤美诚-data.json",
//            "20180526 中国香港公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
//            "20180602 中国公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
//            "20180610 日本公开赛 女单决赛 王曼昱vs伊藤美诚-data.json",
//            "20191005 瑞典公开赛 女单四分之一决赛 王曼昱vs伊藤美诚-data.json",
//            "20191006 瑞典公开赛 女单决赛 陈梦vs伊藤美诚-data.json",
//            "20191006 瑞典公开赛 女单半决赛 孙颖莎vs伊藤美诚-data.json",
//            "20191013 德国公开赛 女单决赛 孙颖莎vs伊藤美诚-data.json",
            "fliter_pingpong.json"
};
    BaseDS::load_file(files, FileType::TableTennis, dir);


    char filename[] = "temp.dat";
    BaseDS::convert_data(filename);


    char *tennis_argv[] = {
                "",
                "-i",
                filename,
                "-w",
                "true"
    };
    int tennis_argc = sizeof(tennis_argv) / sizeof(char *);
    enter_beep(tennis_argc, tennis_argv);

//    char *quantitative_argv[] = {
//            "",
//            "-i",
//            "./quantitative_base.dat",
//            "-w",
//            "true"
//    };
//    int quantitative_argc = sizeof(quantitative_argv) / sizeof(char *);
//    enter_beep(quantitative_argc, quantitative_argv);
}
