#include"base.h"
#include"ReadCfg.h"
#include"general.h"
#include"IntelligentScissors.h"
#include"List.h"
#include"GlobalClass.h"

//����ĳ�ʼ������
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
		cerr << "�����ļ���ȡʧ�ܡ�������" << endl;
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
		cout << "����������1/���ζ�0" << endl;
		cin >> method;
		Mat image = imread(ReadCfg::data[i].path);
		intellSci(image);
		++i;
		int order;
		cout << "������1/0"<<endl;
		cin >> order;
		if (order != 1)
			break;
	}
}
