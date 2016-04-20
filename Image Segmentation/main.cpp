#include"base.h"
#include"ReadCfg.h"
#include"general.h"
#include"IntelligentScissors.h"
#include"List.h"

//程序的初始化函数
void init()
{
    ReadCfg readCfg;
}

void showHelp()
{
	const char* path = "readme.txt";
	fstream fread(path, ios::in);
	if (!fread.is_open())
	{
		cerr << "帮助文件读取失败…………" << endl;
		return;
	}
	fread.seekg(0, ios::end);
	unsigned int length = fread.tellg();
	char* buffer = new char[length-2];
	fread.seekg(0, ios::beg);
	fread.read(buffer, length);
	buffer[length-3] = '\0';
	fread.close();
	cout << buffer << endl;
	delete[] buffer;
}

struct C
{
	static bool prior(int a, int b)
	{
		return (a < b);
	}
};


int main()
{ 

	init();

	int length = ReadCfg::data.size();
	int i = 0;
	while (length--)
	{
		Mat image = imread(ReadCfg::data[i].path, 1);
		intellSci(image);
		++i;
		int order;
		cout << "继续？1/0"<<endl;
		cin >> order;
		if (order != 1)
			break;
	}
}
