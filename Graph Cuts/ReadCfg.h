#ifndef READCFG
#define READCFG
#include<vector>
#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;

extern const char* PATH;

//图像路径记录
struct ImgCfg
{
    int imgId;
    string path;
    ImgCfg(int x):imgId(x) {}
};


//读入图像配置文件的类
class ReadCfg
{
    public:
    static vector<ImgCfg> data;
    ReadCfg();
    ~ReadCfg();
    void init(const char* path = PATH);
};

#endif