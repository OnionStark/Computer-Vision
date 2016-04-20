#include"ReadCfg.h"
#include<cstdlib>
const char* PATH = "img.cfg";

vector<ImgCfg> ReadCfg::data;

int charToInt(const char* buffer, int l, int r)
{
    int result = 0;
    while(l<r)
    {
        result = result*10+buffer[l]-'0';
        ++l;
    }
    return result;
}

//忽略没有意义的字符
void ignore(const char* buffer, int& index)
{
    while(buffer[index] == '#' || buffer[index] == ' ' || buffer[index] == '\n')
    {
        while(buffer[index] == '#')
        {
            while(buffer[index++]!='\n');
        }
        while(buffer[index] == ' ' || buffer[index] == '\n') ++index;
    }
}

void getData(vector<ImgCfg> &data, const char* buffer, int& index)
{
    //从当前位置读入一个ImgCfg
    int tempi = index;
    int k=10;
    while(buffer[tempi]!=' ' && buffer[tempi]!=':') ++tempi;
    data.push_back( ImgCfg(charToInt(buffer, index, tempi) ) );
    //忽略掉没有意义的字符 ' ' ':'
    index = tempi;
    while(buffer[index] == ' ' || buffer[index] == ':' ) ++index;
    tempi = index;
    while(buffer[index] != '\n' && buffer[index]!=' ' ) ++index;
    data.back().path = string(buffer+tempi, index-tempi);
}



ReadCfg::ReadCfg()
{
    init();
}
ReadCfg::~ReadCfg()
{
}

void ReadCfg::init(const char* path/* = PATH*/)
{
    ifstream fread(path, ios::in);
    if(!fread.is_open())
    {
        cerr<<path<<"can not open!"<<endl;
        exit(1);
    }
    
    fread.seekg(0, ios::end);
    int length = static_cast<int>(fread.tellg());
    char* buffer = new char[length+1];
    buffer[length] = '\0';
    fread.seekg(0, ios::beg);
    fread.read(buffer, length);
    fread.close();
    
    int k=1;
    //deal
    int i = 0;
    while(true)
    {
        ignore(buffer, i);
        //测试是不是到达结尾
        if(buffer[i] == 'e' && buffer[i+1] == 'n' && buffer[i+2] == 'd') break;
        //开始读入数据
        getData(data, buffer, i);
    }
        
    delete[] buffer;
}