#ifndef _GRAPHCUT
#define _GRAPHCUT
#include"base.h"
#include"Edge.h"
typedef vector<int> vint;

class GraphCut
{
	Mat* img;
	Edge* pointE;//每个像素点的边
	Edge* sourceE;//s点的输出边
	char* mark;//标记像素是o 还是 b 


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

//graphcut的启动函数
void graphcut(Mat& img);

//鼠标函数
void mouse_function(int mEvent, int x, int y, int flags, void* param);

#endif