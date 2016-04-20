#include"Gaussy.h"
#include<iostream>
using namespace std;
#include<cmath>

const double TWOSQARTPI = 0.159154943;
const double SQARTPI = 0.39894228;
const float inversePI = 0.318309886f;//PI 的倒数


//获取一维高斯的值
inline double getGaussion(float k, int x)
{
    return (SQARTPI*(1.0/k)*exp( -(x*x)/(2.0*k*k) ) );
}
inline double getGaussion(float k, int x, int y)
{
    double kk = k*k;
    return (TWOSQARTPI*(1.0/kk)*exp( -(x*x +y*y)/2.0/kk ) );
}

//--------------xjMat----------------------

xjMat::xjMat(unsigned char* d, int r, int c, int inc):
    data(d), rows(r), cols(c), channels(inc)
{
    
}


xjMat::xjMat(const Mat& M)
{
    if(!M.isContinuous())
    {
        cerr<<"error, the matrix's data is not continuous!"<<endl;
        exit(1);
    }
    data = M.data;
    rows = M.rows;
    cols = M.cols;
    channels = M.channels();
}





//------------GaussSeparate-------------------

GaussSeparate::GaussSeparate(float keno)
{
    init(keno);
}

void GaussSeparate::init(float keno)
{
    this->keno = keno;
    length = static_cast<int>(6*keno+1);
    if( (length&1) == 0) ++length;
    x = new double[length];
    
    //生成对应的模板
    int h = length>>1;
    double sum = 0;
    for(int i=0;i<=h; ++i)
    {
        x[i] = getGaussion(keno, i-h);
        sum += x[i] + x[i];
    }
    sum -= x[h];
    sum += sum;
    //归一化
    for(int i=0; i<=h; ++i)
    {
        x[i] = x[i] / sum;
        x[length-1-i] = x[i];
    }
}

void GaussSeparate::working(Mat& M, const Mat& source)
{
    xjMat target(M.data, source.rows, source.cols, source.channels());
    const xjMat s(source);
    return working(target, s);
}
void GaussSeparate::working(xjMat& target, const xjMat& source)
{
    for(int i=0;i<source.rows;++i)
    {
        for(int j=0; j<source.cols; ++j)
        {
            double sum[3] = {0.0, 0.0, 0.0};
            int h = length>>1;
            for(int k=0; k<length; ++k)
            {
                sum[R] += source.get(i, j+k-h, R)*x[k];
                sum[R] += source.get(i+k-h, j, R)*x[k];
                sum[G] += source.get(i, j+k-h, G)*x[k];
                sum[G] += source.get(i+k-h, j, G)*x[k];
                sum[B] += source.get(i, j+k-h, B)*x[k];
                sum[B] += source.get(i+k-h, j, B)*x[k];
            }
            target.set(i,j, R) = static_cast<unsigned char>(sum[R]);
            target.set(i,j, G) = static_cast<unsigned char>(sum[G]);
            target.set(i,j, B) = static_cast<unsigned char>(sum[B]);
        }
    }
}

GaussSeparate::~GaussSeparate()
{
    delete[] x;
}



//-----------GuassTwoDimen--------
GaussTwoDimen::GaussTwoDimen(float keno)
{    
    init(keno);
}

void GaussTwoDimen::init(float keno)
{
    this->keno = keno;
    length = static_cast<int>(6*keno+1);
    if( (length&1)==0 ) ++length;
    
    x = new double[length*length];
    //x[length][length]
    //x[i][j] = i*length+j
    int h = length>>1;
    double sum = 0;
    
    for(int i=0;i<length;++i)
    {
        for(int j=0;j<length; ++j)
        {
            x[i*length +j] = getGaussion(keno, i-h, j-h);
            sum += x[i*length+j];
        }
    }
    
    //归一化
    for(int i=0;i<length; ++i)
    {
        for(int j=0;j<length; ++j)
            x[i*length+j] /= sum;
    }
    
    //----------------
    // for(int i=0;i<length; ++i)
    // {
    //     for(int j=0;j<length; ++j)
    //         cout<<x[i*length+j]<<"  ";
    //     cout<<endl;
    // }
}

GaussTwoDimen::~GaussTwoDimen()
{
    delete[] x;
}

void GaussTwoDimen::working(Mat& target, const Mat& source)
{
    xjMat tar(target.data, source.rows, source.cols, source.channels());
    const xjMat sou(source);
    return working(tar, source);
}
void GaussTwoDimen::working(xjMat& target, const xjMat& source)
{
    if(source.channels==3)
    {
        for(int i=0;i<source.rows;++i)
        {
            for(int j=0;j<source.cols; ++j)
            {
                double sum[3] = {0.0, 0.0, 0.0};
                int h = length>>1;
                for(int a = 0; a<length; ++a)
                {
                    for(int y=0;y<length; ++y)
                    {
                        
                        sum[R] += source.get(i+a-h, j+y-h, R)*x[a*length+y];
                        sum[G] += source.get(i+a-h, j+y-h, G)*x[a*length+y];
                        sum[B] += source.get(i+a-h, j+y-h, B)*x[a*length+y];
                    }
                }
                target.set(i, j, R) = static_cast<unsigned char>(sum[R]);
                target.set(i, j, G) = static_cast<unsigned char>(sum[G]);
                target.set(i, j, B) = static_cast<unsigned char>(sum[B]);
            }
        }
    }
    else if(source.channels==1)
    {
        for(int i=0;i<source.rows;++i)
        {
            for(int j=0;j<source.cols; ++j)
            {
                double sum[3] = {0.0, 0.0, 0.0};
                int h = length>>1;
                for(int a = 0; a<length; ++a)
                {
                    for(int y=0;y<length; ++y)
                    {
                        sum[B] += source.get(i+a-h, j+y-h, B)*x[a*length+y];
                    }
                }
                target.set(i, j, B) = static_cast<unsigned char>(sum[B]);
            }
        }
    }
}


gaussyTwo::gaussyTwo(float k, int m, int n) :keno(k), r(m), c(n)
{
    //假设 m 和 n 都是奇数
    x = new float[m*n];
    int mh = m>>1;
    int nh =n>>1;
    float kk = 2.0f*keno*keno;
    for(int i=0;i<=mh; ++i)
    {
        for(int j =0;j<=nh; ++j)
        {
            x[i*n + n-j-1] =  x[(m-1-i)*n+j] =  x[(m-i-1)*n + n-j-1] = x[i*n+j] = inversePI*kk*exp( ((i-mh)*(i-mh)+(j-nh)*(j-nh) )/-kk);
        }
    }
    
    float sum = 0.0f;
    for(int i=0; i<m*n; ++i)
        sum += x[i];
    
    for(int i=0;i<m*n;++i)
        x[i] /= sum;
}