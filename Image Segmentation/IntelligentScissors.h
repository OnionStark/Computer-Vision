#ifndef _INTELLIGENTSCISSORS
#define _INTELLIGENTSCISSORS
#include"base.h"
#include"Axis.h"

//���ܼ����㷨ִ�к���
void intellSci(Mat& img);


class IntellSci
{
	Mat& img;
	//Fz
	uchar* Fz; //������˹�������
	float* Fg;//�ݶ�ֵ
	Axis<short>* Dp;//D`(p)
	static float Wz, Wd, Wg, Wp, Wi, Wo;
	int* p;//��¼·��
	float* ilc;

	//-------function-----
	void computeFz();
	void computeFgDp();
	float computeFd(int px, int py, int qx, int qy);
	float computeFd(const Axis<short>&p, const Axis<short>& q);
	void computeILC();   //Ԥ�������е�ilcֵ

	void forMethod1();
	void forMethod2();
public:
	IntellSci(Mat& image):Fz(nullptr), Fg(nullptr), Dp(nullptr), img(image),
		p(nullptr), ilc(nullptr) {}
	~IntellSci();
	void init();
	float ILC(int px, int py, int qx, int qy);
	float ILC(const Axis<int>& p, const Axis<int>& q);
	int* getP() const { return p; }
	friend void DP(int sx, int sy,IntellSci* ins);
	friend void showinImg(IplImage*&,IntellSci*, int, int);
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
//���Ļص�������������������
void activePrint(int m_event, int x, int y, int flags, void* param);

//Ѱ·����
void DP(int sx, int sy,IntellSci* ins);

void showinImg(IplImage*&,IntellSci*, int, int);

#endif