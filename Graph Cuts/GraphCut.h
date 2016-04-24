#ifndef _GRAPHCUT
#define _GRAPHCUT
#include"base.h"
#include"Edge.h"
typedef vector<int> vint;

class GraphCut
{
	Mat* img;
	Edge* pointE;//ÿ�����ص�ı�
	Edge* sourceE;//s��������
	char* mark;//���������o ���� b 


	void computeCost();
public:
	GraphCut(Mat* ini = nullptr) :img(ini), pointE(nullptr), sourceE(nullptr),
		mark(nullptr) {}
	~GraphCut();
	void init();
	void clearMark()
	{
		if (mark == nullptr) return;
		int N = img->rows*img->cols;
		for (int i = 0; i < N; ++i)
		{
			mark[i] = 'p';
		}
	}
	void IamWorking();
	char* getMark();
};


struct FuncParam
{
	GraphCut* graphcut;
	IplImage* img;
};

//graphcut����������
void graphcut(Mat& img);

//��꺯��
void mouse_function(int mEvent, int x, int y, int flags, void* param);

#endif