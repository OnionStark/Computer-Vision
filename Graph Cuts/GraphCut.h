#ifndef _GRAPHCUT
#define _GRAPHCUT
#include"base.h"
#include"Edge.h"
#include"GlobalClass.h"
typedef vector<int> vint;
struct MarkCut;
class GraphCut
{
	Mat* img;
	Edge* pointE;//每个像素点的边
	Edge* sourceE;//s点的输出边
	char* mark;//标记像素是o 还是 b 
	vint result;

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
			mark[i] = getGlobal()->nullMark;
		}
	}
	void IamWorking();
	char* getMark() { return mark; }
	Edge* getEdge(int u, int v);//获取uv 之间的边
	friend void minCut(GraphCut*);
	friend void showInImage(GraphCut*, IplImage*,MarkCut*);
};

struct MarkCut
{
	int p;//标记的点
	float f;//标记的流
	MarkCut(int inp = 0, float inf = -1.f) :p(inp), f(inf) {}
	//如果f==-1，表示并没有被标记
	bool isMark() const { return (f != -1.f); }
	static void clear(MarkCut* temp, int length)
	{
		for (int i = 0; i < length; ++i)
		{
			temp[i].p = 0;
			temp[i].f = -1.f;
		}
	}
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

//最小割算法
void minCut(GraphCut*);

//显示结果
void showInImage(GraphCut*, IplImage*,MarkCut*);
#endif