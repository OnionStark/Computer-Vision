#ifndef _GENERAL
#define _GENERAL
#include"base.h"

//����ȡ����ֻȡ������
template<typename T>
inline int roundUp(T x)
{
	if (x < 0) return 0;
	int y = static_cast<int>(x);
	return (x - y == 0.f ? y : y + 1);
}

//���image Ϊ3ͨ��ͼ����ת��Ϊ�Ҷ�ͼ
void converToGray(Mat& target, const Mat& image);

//˫���Բ�ֵ
void bilinear(Mat& target,  const Mat& image, float scale);

//���㽵����
void downSampling(Mat& target, const Mat& image);

//��������ͼƬ�Ĳ�ֵ
//target = image1-image2;
void differ(Mat& target, const Mat&image1, const Mat& image2);

//���׾�������
void inverseMatrix(float[3][3]);

//��ǰ�����������ĵ��
void matrixDot(float[3][3], float[3], float[3]);

//��ʾĳ��ͼ��
void show(const string path, const Mat& im);

//ͼ��ĸ���
void cloneImage(IplImage* target, const IplImage* source);

//��mat ��Ϊ������
void converToContinue(Mat* m);

//Ȧ��ĳ��ͼ��ķ�Χ��Ȼ�������Щ��
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
				function(target, tx, ty);//�����������һ�㲻Ϊ��֪�ı���
			else continue;
		}
	}
}

#endif 