#include"Sift.h"
#include"base.h"
#include"Gaussy.h"
#include"Parymid.h"
const float SIFTParam::keno = 0.5;
const int SIFTParam::s = 3;
int SIFTParam::S = s+3;
int SIFTParam:: O = 0;
float SIFTParam::k = 0.0;
const int SIFTParam::SIFT_MAX_STEPS = 5;
const float SIFTParam::r = 10.0f;// 记录角点的经验值
const float SIFTParam::TT = 0.04;
//-----------SIFT--------------

inline int min(int x, int y)
{
    return (x>y?y:x);
}
inline double log2(int x)
{
    return (log(x)/log(2));
}
void SIFTParam::init(int x, int y)
{
    S = s+3;
    O = log2( min(x, y) ) - 2;
    k = pow(2.0, 1.0/s);
}

void paintImg(xjMat& target, int x, int y, int xend, int yend)
{
    ;
}

//用箭头记录方向
void markI(Mat& image, int x, int y, ExtremePoint& octave)
{
    xjMat target(image);
    int xoffset = octave.getAm()*sin( octave.getDirection() );
    int yOffset = octave.getAm()*cos( octave.getDirection() );
    //paintImg(target, x, y, x+xOffset, y+yOffset);
}

//以特征点标记图像
void markImg(Mat& image, int x, int y)
{
    xjMat temp(image);
    // temp.set(x, y) = 255;
    // return;
    
      //以 + 号标记 5×5 大小
    int h = 1;
    for(int m = x-h; m<=x+h; ++m)
    {
        if(m<0 || m >= image.rows)  continue;
        temp.set(m, y, R) = 0;
        temp.set(m, y, G) = 255;
        temp.set(m, y, B) = 0;
    }
    for(int n = y-h; n<=y+h; ++n)
    {
        if(n<0 || n>=image.cols )  continue;
        temp.set(x, n, R) = 0;
        temp.set(x, n, G) = 255;
        temp.set(x, n, B) = 0;
    }
}

void test(Mat& image, ImgOctave<SIFTParam>& octave )
{
    Mat temp;
    temp.create(image.rows, image.cols, CV_8UC3);
    xjMat mtemp(temp);
    xjMat source(image);
    for(int i=0;i<image.rows; ++i)
    {
        for(int j=0; j<image.cols; ++j)
        {
            auto v = source.get(i, j);
            mtemp.set(i, j, R) = v; 
            mtemp.set(i, j, G) = v; 
            mtemp.set(i, j, B) = v; 
        }
    }
    
    //在图像上标记特征点
    auto point = octave.getExtremePoint();
    int a[2];    
    
    for(int i=0; i<point.size(); ++i)
    {
        point[i].getActuallyXY(a);
        markImg(temp, a[0], a[1]);
        //markI(temp, a[0], a[1], point[i]);
    }
    show("haha", temp);
}


void SIFT_DIY(Mat& image)
{    
    //先将图像扩大一倍得到第一组，然后再生成金字塔。
    SIFTParam::init(image.rows, image.cols);
    
    //这里建立金字塔，在图像的构造函数形成
    cout<<"正在构建图像高斯金字塔..."<<endl;
    ImgOctave<SIFTParam> octave(image);
    cout<<"正在构建DOG 算子..."<<endl;
    //产生dog算子
    octave.produceDog();

    //计算极值点，并且产生特征点        
    cout<<"正在确定特征点..."<<endl;
    octave.computePoint();
    
    cout<<"正在确定特征点的方向角度..."<<endl;
    octave.computeAm();//计算角度和振幅
    
    
    
    test(image, octave);
    return;
    
    
    //建立金字塔
    
    //从尺度为0.5 开始构建图像的金字塔
    GaussTwoDimen* g = new GaussTwoDimen[SIFTParam::S];
    Mat* temp = new Mat[SIFTParam::S];
    float kmi = 1.0;
    for(int i=0; i<SIFTParam::S; ++i)
    {
        temp[i] = image.clone();
        //初始化高斯算子
        g[i].init(kmi*SIFTParam::keno);
        g[i].working(temp[i], image);
        kmi *= SIFTParam::k;
    }
    
    char name[] = "leno0";
    int len = strlen(name);
    for(int i=0;i<SIFTParam::S; ++i)
    {
        name[len-1] = i+'0';
        namedWindow(name, WINDOW_AUTOSIZE);
        imshow(name, temp[i]);
        waitKey(0);
    }
    
    
    delete[] g;
    delete[] temp;
    
    //----------------------------------------------
    // Gaussy* g = new GaussTwoDimen(3.0);
    // //Gaussy* g = new GaussSeparate(5.0);
    // Mat temp = image.clone();
    // g->working(temp, image);
    // namedWindow("LenaGau.jpg", WINDOW_AUTOSIZE);
    // imshow("LenaGau.jpg", temp);
    // waitKey(0);
    // //imwrite("img/Lena的二维高斯模糊_0.6.jpg", temp);
    // delete g;
}