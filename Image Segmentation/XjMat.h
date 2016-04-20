#ifndef _XJMAT
#define _XJMAT
#include"base.h"
//自定义的矩阵
struct xjMat
{
	unsigned char* data;
	int rows, cols, channels;
public:
	xjMat(const Mat& M);
	xjMat(unsigned char*, int inrows, int incols, int inc);
	inline unsigned char get(int i, int j, int k = 0) const
	{
		//为了追求效率，这里不进行下标检查
		if (i<0 || j<0 || i >= rows || j >= cols) return 0;
		else return *(data + i*cols*channels + j*channels + k);
	}
	inline unsigned char& set(int i, int j, int k = 0)
	{
		return *(data + i*cols*channels + j*channels + k);
	}
};

#endif