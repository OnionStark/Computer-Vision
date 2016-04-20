#ifndef _GAUSSY
#define _GAUSSY
#include"base.h"
//#include"Parymid.h"

//�Զ���ľ���
struct xjMat
{
    unsigned char* data;
    int rows, cols, channels;
    public:
    xjMat(const Mat& M);
    xjMat(unsigned char* ,int inrows, int incols, int inc);
    inline unsigned char get(int i, int j, int k=0) const
    {
            //Ϊ��׷��Ч�ʣ����ﲻ�����±���
        if(i<0 || j<0 || i>=rows || j >= cols) return 0;
        else return *(data + i*cols*channels + j*channels +k);
    }
    inline unsigned char& set(int i, int j, int k =0)
    {
        return *(data + i*cols*channels + j*channels +k);
    }
};


//��˹���ӳ�����
class Gaussy
{
    protected:
    
    float keno;
    int length;
    
    public:
    Gaussy(){}
    virtual ~Gaussy(){}
    virtual void init(float keno) = 0;
    virtual void working(Mat& M, const Mat& source) = 0;
    virtual void working(xjMat&, const xjMat& ) = 0;
};


//�����˹ģ��������
class GaussSeparate:public Gaussy
{
    //�ֱ�����ά�����ģ��
    //��Χ��(6keno+1)*(6keno+1);
    double* x;
    public:
    GaussSeparate(){}
    GaussSeparate(float keno);
    ~GaussSeparate();
    void init(float keno);
    
    //��ͼ����и�˹ģ�����������M ��
    void working(Mat& M, const Mat& source);
    void working(xjMat& M, const xjMat& source);
};

//��ά��˹ģ��������
class GaussTwoDimen:public Gaussy
{
    double* x;
    public:
    GaussTwoDimen(){}
    GaussTwoDimen(float keno);
    ~GaussTwoDimen();
    void init(float keno);
    
    //��ͼ����и�˹ģ�����������M ��
    void working(Mat& M, const Mat& source);
    void working(xjMat& M, const xjMat& source);
};

class gaussyTwo
{
    float* x;
    float keno;
    int r, c;
    public:
    gaussyTwo(float k, int m, int n);
    virtual ~gaussyTwo()
    {
        delete[] x;
    }
    inline float operator () (int m, int n)
    {
        m += r>>1;
        n += c>>1;
        
        return x[m*c+n];
    }
};
#endif