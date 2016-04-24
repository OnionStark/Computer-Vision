#ifndef _GENERAL
#define _GENERAL
#include"base.h"

//向上取整，只取正整数
template<typename T>
inline int roundUp(T x)
{
	if (x < 0) return 0;
	int y = static_cast<int>(x);
	return (x - y == 0.f ? y : y + 1);
}

//如果image 为3通道图，则转化为灰度图
void converToGray(Mat& target, const Mat& image);

//双线性插值
void bilinear(Mat& target,  const Mat& image, float scale);

//隔点降采样
void downSampling(Mat& target, const Mat& image);

//计算两张图片的差值
//target = image1-image2;
void differ(Mat& target, const Mat&image1, const Mat& image2);

//三阶矩阵求逆
void inverseMatrix(float[3][3]);

//求前面两个向量的点乘
void matrixDot(float[3][3], float[3], float[3]);

//显示某张图像
void show(const string path, const Mat& im);

//图像的复制
void cloneImage(IplImage* target, const IplImage* source);

//把mat 变为连续的
void converToContinue(Mat* m);

//圈定某个图像的范围，然后遍历这些点
template<typename F>
void printRadiu(IplImage* target, int x, int y, float R, F function)
{
	int bx = roundUp(x + R);
	int by = roundUp(y + R);
	R *= R;
	for (int ty = roundUp(y - R); ty <= by; ++ty)
	{
		for (int tx = roundUp(x - R); tx <= bx; ++tx)
		{
			if( (tx-x)*(tx-x)+(ty-y)*(ty-y) <=R )
				function(target, tx, ty);//对这个像素做一点不为人知的遍历
			else continue;
		}
	}
}

#endif 