#ifndef READCFG
#define READCFG
#include<vector>
#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;

extern const char* PATH;

//ͼ��·����¼
struct ImgCfg
{
    int imgId;
    string path;
    ImgCfg(int x):imgId(x) {}
};


//����ͼ�������ļ�����
class ReadCfg
{
    public:
    static vector<ImgCfg> data;
    ReadCfg();
    ~ReadCfg();
    void init(const char* path = PATH);
};

#endif