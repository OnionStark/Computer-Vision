#ifndef _INTELLIGENTSCISSORS
#define _INTELLIGENTSCISSORS
#include"base.h"
#include"Axis.h"

//智能剪刀算法执行函数
void intellSci(Mat& img);


class IntellSci
{
	Mat& img;
	//Fz
	uchar* Fz; //拉普拉斯交叉零点
	float* Fg;//梯度值
	Axis<short>* Dp;//D`(p)
	static float Wz, Wd, Wg, Wp, Wi, Wo;
	int* p;//记录路径

	//-------function-----
	void computeFz();
	void computeFgDp();
	float computeFd(int px, int py, int qx, int qy);
	float computeFd(const Axis<short>&p, const Axis<short>& q);
public:
	IntellSci(Mat& image):Fz(nullptr), Fg(nullptr), Dp(nullptr), img(image), p(nullptr) {}
	~IntellSci();
	void init();
	float ILC(int px, int py, int qx, int qy);
	float ILC(const Axis<int>& p, const Axis<int>& q);
	friend void DP(int sx, int sy, IntellSci* ins);
};

struct ActiveParam
{
	IntellSci* ins;
	IplImage* image;
};

struct Compare
{
	static float* g;
	static bool prior(const int a, const int b)
	{
		return (g[a] < g[b]);
	}
};
//鼠标的回调函数，用来绘制线条
void activePrint(int m_event, int x, int y, int flags, void* param);

//寻路函数
void DP(int sx, int sy, IntellSci* ins);

#endif