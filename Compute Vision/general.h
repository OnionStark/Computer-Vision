#ifndef _GENERAL
#define _GENERAL
#include"base.h"
#include"Gaussy.h"

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

#endif 