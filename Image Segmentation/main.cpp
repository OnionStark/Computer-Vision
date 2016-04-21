#include"base.h"
#include"ReadCfg.h"
#include"general.h"
#include"IntelligentScissors.h"
#include"List.h"
#include"GlobalClass.h"

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
	char* buffer = new char[length+1];
	fread.seekg(0, ios::beg);
	fread.read(buffer, length);
	buffer[length] = '\0';
	fread.close();
	cout << buffer << endl;
	delete[] buffer;
}

int main()
{ 

	init();
	showHelp();

	int length = ReadCfg::data.size();
	int i = 0;
	while (length--)
	{
		getGlobal().reset();
		cout << "方法？论文1/康奈尔0" << endl;
		cin >> method;
		Mat image = imread(ReadCfg::data[i].path);
		intellSci(image);
		++i;
		int order;
		cout << "继续？1/0"<<endl;
		cin >> order;
		if (order != 1)
			break;
	}
}
