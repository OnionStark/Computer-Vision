#ifndef _GLOABLCLASS
#define _GLOBALCLASS
#include"base.h"
#include"IntelligentScissors.h"
//ȫ���࣬����ģʽ�����ڿ��ƽ�������ʾ����
class GlobalClass
{

	GlobalClass() :imgptr(nullptr) {}
	GlobalClass(const GlobalClass&) {} //����ģʽ���������������
public:


	IplImage* imgptr;   //������ʾ��ͼ��ָ��
	bool isFirst;   //���ڵ�һ��������С·��ͼ
	int px, py;

	~GlobalClass();
	void reset();   //ÿ���������ó�ֵ
	void setImage(const IplImage*);
	void updateImg(int, int, IntellSci*);  //����ͼ����Ϣ�ĺ���
	friend GlobalClass& getGlobal();
};
inline GlobalClass& getGlobal()
{
	static GlobalClass g;
	return g;
}


#endif _GLOBALCLASS