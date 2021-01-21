#include <iostream>
#include "base/FileLoader.hpp"
#include <ctime>
#include "FMP/Fmp.hpp"
#include "DITTO/Ditto.h"

using namespace std;

list<BaseSequence> FMP::sequenceList = {};
int FmpPattern::totalTimeStamp = 0;
CodeTableType FMP::codeTable = {};

void test_ditto();

const int BaseSequence::coverMissFlag = -1;
const int FmpPattern::patternNULLFlag = -1;

void load_file() {
    string dir = "../data/tt/";
    vector<string> files = {
            "20180324 德国公开赛 女单第二轮 孙颖莎vs伊藤美诚-data.json",
            "20180526 中国香港公开赛 女单半决赛 王曼昱vs伊藤美诚-data.json",
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
    clock_t t1 = clock();
    for (const auto &file: files)
        FMP::sequenceList.splice(FMP::sequenceList.end(), FileLoader::loadFile(dir + file, FileType::TableTennis));
    cout << "Finish Loading! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
    cout << "BaseAttribute mapping:" << endl << BaseAttribute::get_record() << endl;
    cout << "Total sequences: " << FMP::sequenceList.size() << endl;
}

void test_fmp() {
    clock_t t1 = clock();
    FMP::do_fmp();
    cout << endl << "Finish FMP! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
}

void test_ditto() {
    //region convert data
    string filename = "temp.dat";
    ofstream f;
    f.open(filename, ios::out);
    auto attrs = BaseAttribute::get_keys();
    int nrAttr = attrs.size();
    f << nrAttr;
    for (const auto &attr: attrs)
        f << " " << BaseAttribute::get_attrs(attr).size();
    for (int i = 0; i < nrAttr; i++) {
        f << endl;
        for (auto sequence: FMP::sequenceList) {
            for (auto &event : sequence) {
                f << event[i] << " ";
            }
        }
        if (i != nrAttr - 1) f << -2;
    }
    f.close();
    //endregion

    //region run
    char* cstr = new char[filename.length() + 1];
    strcpy(cstr, filename.c_str());
    char *argv[] = {
            "",
            "-i",
            cstr,
            "-w",
            "true"
    };
    int argc = sizeof(argv) / sizeof(char *);
    clock_t t1 = clock();
    dittoEnter(argc, argv);
    cout << endl << "Finish DITTO! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;

//    clock_t t1 = clock();
//    system(("..\\DITTO\\Ditto.exe -i " + filename + " -w true").c_str());
//    cout << endl << "Finish DITTO! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
    //endregion

    //region remove temp file
    remove(filename.c_str());
    //endregion
}

int main() {
    load_file();

//    test_fmp();
    test_ditto();
}
