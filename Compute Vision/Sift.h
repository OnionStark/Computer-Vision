#ifndef _SIFT
#define _SIFT
#include"base.h"

//SIFT�㷨�ĳ���Ĭ�ϲ���
class SIFTParam
{
    //�����캯����Ϊ˽�У������������ʵ��
    private:
    SIFTParam(){}
    ~SIFTParam(){}
    public:
    static const float keno;//��0.5 ��ʼ����������, ��׼�߶�
    static const int s;// s=0.3
    static int O; //������
    static int S;// S = s+3 ,ÿ��ͼ����ٲ�
    static float k;// ÿ���߶������Ĳ��� k = 2^1/s
    static const int SIFT_MAX_STEPS;  //�����������ʱ����ߵĵ�������
    static const float r;// ��¼�ǵ�ľ���ֵ
    static const float TT; //���������Ӧ������ֵ
    
    //@param x,y ��ͼ��Ŀ�͸�
    static void init(int x, int y);//��ʼ����ǰ���㷨����
};

void SIFT_DIY(Mat& image);

#endif