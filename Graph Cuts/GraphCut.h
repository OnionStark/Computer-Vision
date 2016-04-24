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
	Edge* pointE;//ÿ�����ص�ı�
	Edge* sourceE;//s��������
	char* mark;//���������o ���� b 
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
	Edge* getEdge(int u, int v);//��ȡuv ֮��ı�
	friend void minCut(GraphCut*);
	friend void showInImage(GraphCut*, IplImage*,MarkCut*);
};

struct MarkCut
{
	int p;//��ǵĵ�
	float f;//��ǵ���
	MarkCut(int inp = 0, float inf = -1.f) :p(inp), f(inf) {}
	//���f==-1����ʾ��û�б����
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

//graphcut����������
void graphcut(Mat& img);

//��꺯��
void mouse_function(int mEvent, int x, int y, int flags, void* param);

//��С���㷨
void minCut(GraphCut*);

//��ʾ���
void showInImage(GraphCut*, IplImage*,MarkCut*);
#endif