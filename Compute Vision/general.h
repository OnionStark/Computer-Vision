#ifndef _GENERAL
#define _GENERAL
#include"base.h"
#include"Gaussy.h"

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

#endif 