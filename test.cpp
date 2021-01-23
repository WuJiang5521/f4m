#include <iostream>
#include "DITTO/Ditto.h"
#include "base/BaseDS.hpp"
#include "FMP/Fmp.hpp"

using namespace std;

int main() {
    string dir = "../data/tt/";
    vector<string> files = {
            "20180324 德国公开赛 女单第二轮 孙颖莎vs伊藤美诚-data.json",
//            "20180526 中国香港公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
//            "20180602 中国公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
//            "20180610 日本公开赛 女单决赛 王曼昱vs伊藤美诚-data.json",
//            "20180727 澳大利亚公开赛 女单四分之一决赛 何卓佳vs伊藤美诚-data.json",
//            "20181103 瑞典公开赛 女单半決赛 丁宁vs伊藤美诚-data.json",
//            "20181103 瑞典公开赛 女单四分之一决赛 刘诗雯vs伊藤美诚-data.json",
//            "20181104 瑞典公开赛 女单决赛 朱雨玲vs伊藤美诚-data.json",
//            "20190330 卡塔尔公开赛 女单八分之一决赛 丁宁vs伊藤美诚-data.json",
//            "20190424 布达佩斯世乒赛 女单十六分之一决赛 孙颖莎vs伊藤美诚-data.json",
//            "20190530 中国公开赛 女单十六分之一决赛 王艺迪vs伊藤美诚-data.json",
//            "20190601 中国公开赛 女单四分之一决赛 丁宁vs伊藤美诚-data.json",
//            "20190602 中国公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
//            "20190609 香港公开赛 女单决赛 王艺迪vs伊藤美诚-data.json",
//            "20190706 韩国公开赛 女单四分之一决赛 王曼昱vs伊藤美诚-data.json",
//            "20190713 澳大利亚公开赛 女单半决赛 丁宁vs伊藤美诚-data.json",
//            "20190818 保加利亚公开赛 女单半决赛 陈幸同vs伊藤美诚-data.json",
//            "20190824 捷克公开赛 女单八分之一决赛 陈幸同vs伊藤美诚-data.json",
//            "20191005 瑞典公开赛 女单四分之一决赛 王曼昱vs伊藤美诚-data.json",
//            "20191006 瑞典公开赛 女单决赛 陈梦vs伊藤美诚-data.json",
//            "20191006 瑞典公开赛 女单半决赛 孙颖莎vs伊藤美诚-data.json",
//            "20191013 德国公开赛 女单决赛 孙颖莎vs伊藤美诚-data.json"
    };
    BaseDS::load_file(files, FileType::TableTennis, dir);

    char filename[] = "temp.dat";
    BaseDS::convert_data(filename);

    char *argv[] = {
            "",
            "-i",
            filename,
            "-w",
            "true"
    };
    int argc = sizeof(argv) / sizeof(char *);

    fmpEnter(argc, argv);
    dittoEnter(argc, argv);
}
