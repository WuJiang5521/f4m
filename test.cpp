#include <iostream>
#include "FileLoader.hpp"
#include <time.h>

using namespace std;

int main() {
    string dir = "./data/tt/";
    vector<string> files = {
            "20180324 �¹������� Ů���ڶ��� ��ӱɯvs��������-data.json",
            "20180526 �й���۹����� Ů������� ������vs��������-data.json",
            "20180602 �й������� Ů������� ������vs��������-data.json",
            "20180610 �ձ������� Ů������ ������vs��������-data.json",
            "20180727 �Ĵ����ǹ����� Ů���ķ�֮һ���� ��׿��vs��������-data.json",
            "20181103 ��乫���� Ů����Q�� ����vs��������-data.json",
            "20181103 ��乫���� Ů���ķ�֮һ���� ��ʫ��vs��������-data.json",
            "20181104 ��乫���� Ů������ ������vs��������-data.json",
            "20190330 ������������ Ů���˷�֮һ���� ����vs��������-data.json",
            "20190424 ������˹��ƹ�� Ů��ʮ����֮һ���� ��ӱɯvs��������-data.json",
            "20190530 �й������� Ů��ʮ����֮һ���� ���յ�vs��������-data.json",
            "20190601 �й������� Ů���ķ�֮һ���� ����vs��������-data.json",
            "20190602 �й������� Ů������� ������vs��������-data.json",
            "20190609 ��۹����� Ů������ ���յ�vs��������-data.json",
            "20190706 ���������� Ů���ķ�֮һ���� ������vs��������-data.json",
            "20190713 �Ĵ����ǹ����� Ů������� ����vs��������-data.json",
            "20190818 �������ǹ����� Ů������� ����ͬvs��������-data.json",
            "20190824 �ݿ˹����� Ů���˷�֮һ���� ����ͬvs��������-data.json",
            "20191005 ��乫���� Ů���ķ�֮һ���� ������vs��������-data.json",
            "20191006 ��乫���� Ů������ ����vs��������-data.json",
            "20191006 ��乫���� Ů������� ��ӱɯvs��������-data.json",
            "20191013 �¹������� Ů������ ��ӱɯvs��������-data.json"
    };
    list<Sequence> sequenceList = {};
    clock_t t1 = clock();
    for (const auto& file: files)
        sequenceList.splice(sequenceList.end(), FileLoader::loadFile(dir + file, FileType::TableTennis));
    cout << "Finish Loading! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
    cout << "Attribute mapping:" << endl << Attribute::get_record() << endl;
    cout << "Total sequences: " << sequenceList.size() << endl;
//    for (auto sequence = sequenceList.begin(); sequence != sequenceList.end(); sequence++)
//        cout << distance(sequenceList.begin(), sequence) << "\t: " << *sequence << endl;
}
