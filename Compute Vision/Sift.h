#ifndef _SIFT
#define _SIFT
#include"base.h"

//SIFT算法的常量默认参数
class SIFTParam
{
    //将构造函数设为私有，不允许有类的实例
    private:
    SIFTParam(){}
    ~SIFTParam(){}
    public:
    static const float keno;//从0.5 开始构建金字塔, 基准尺度
    static const int s;// s=0.3
    static int O; //多少组
    static int S;// S = s+3 ,每组图像多少层
    static float k;// 每个尺度中相差的参数 k = 2^1/s
    static const int SIFT_MAX_STEPS;  //计算特征点的时候最高的迭代次数
    static const float r;// 记录角点的经验值
    static const float TT; //特征点的响应经验阈值
    
    //@param x,y 是图像的宽和高
    static void init(int x, int y);//初始化当前的算法参数
};

void SIFT_DIY(Mat& image);

#endif