#ifndef _XJMAT
#define _XJMAT
#include"base.h"
//�Զ���ľ���
struct xjMat
{
	unsigned char* data;
	int rows, cols, channels;
public:
	xjMat(const Mat& M);
	xjMat(unsigned char*, int inrows, int incols, int inc);
	inline unsigned char get(int i, int j, int k = 0) const
	{
		//Ϊ��׷��Ч�ʣ����ﲻ�����±���
		if (i<0 || j<0 || i >= rows || j >= cols) return 0;
		else return *(data + i*cols*channels + j*channels + k);
	}
	inline unsigned char& set(int i, int j, int k = 0)
	{
		return *(data + i*cols*channels + j*channels + k);
	}
};

#endif