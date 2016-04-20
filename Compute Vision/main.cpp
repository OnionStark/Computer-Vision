#include"base.h"
#include"exByOpencv.h"
#include"ReadCfg.h"
#include"Sift.h"
#include"Gaussy.h"
#include"general.h"
#include"Parymid.h"

//����ĳ�ʼ������
void init()
{
    ReadCfg readCfg;
}

//�Լ�д��sift �㷨
int SIFTBymyself()
{
	//����ͼƬ����������˹��άģ��
	Mat image;
	image = imread(ReadCfg::data[0].path, 1);
	if (!image.data)
	{
		cout << "error!" << endl;
		return -1;
	}
	Mat temp;
	if (image.channels() >= 3)
		converToGray(temp, image);

	SIFT_DIY(temp);
	//imwrite("img/jobs.jpg", image);
	namedWindow("Lena.jpg", WINDOW_AUTOSIZE);
	imshow("Lena.jpg", temp);
	waitKey(0);
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
	char* buffer = new char[length-2];
	fread.seekg(0, ios::beg);
	fread.read(buffer, length);
	buffer[length-3] = '\0';
	fread.close();
	cout << buffer << endl;
	delete[] buffer;
}


int main()
{   
	init();
	if (ReadCfg::data.size() < 2)
	{
		cerr << "��������ͼ���·����Ϣ��in img.cfg��" << endl;
		return -1;
	}
	//SIFTBymyself();
	//return 0;
	int exCode;
	showHelp();
	while (true)
	{
		cout << "Please enter excusive code:";
		cin >> exCode;
		switch (exCode)
		{
		case 0:
			return 0;
		case 1:
			sift();
			break;
		case 2:
			surf();
			break;
		case 3:
			fast();
		case 4:
			orb();
			break;
		case 5:
			SIFTBymyself();
			break;
		default :
			;
		}
	}
}