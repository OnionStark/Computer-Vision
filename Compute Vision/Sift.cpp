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
const float SIFTParam::r = 10.0f;// ��¼�ǵ�ľ���ֵ
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

//�ü�ͷ��¼����
void markI(Mat& image, int x, int y, ExtremePoint& octave)
{
    xjMat target(image);
    int xoffset = octave.getAm()*sin( octave.getDirection() );
    int yOffset = octave.getAm()*cos( octave.getDirection() );
    //paintImg(target, x, y, x+xOffset, y+yOffset);
}

//����������ͼ��
void markImg(Mat& image, int x, int y)
{
    xjMat temp(image);
    // temp.set(x, y) = 255;
    // return;
    
      //�� + �ű�� 5��5 ��С
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
    
    //��ͼ���ϱ��������
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
    //�Ƚ�ͼ������һ���õ���һ�飬Ȼ�������ɽ�������
    SIFTParam::init(image.rows, image.cols);
    
    //���ｨ������������ͼ��Ĺ��캯���γ�
    cout<<"���ڹ���ͼ���˹������..."<<endl;
    ImgOctave<SIFTParam> octave(image);
    cout<<"���ڹ���DOG ����..."<<endl;
    //����dog����
    octave.produceDog();

    //���㼫ֵ�㣬���Ҳ���������        
    cout<<"����ȷ��������..."<<endl;
    octave.computePoint();
    
    cout<<"����ȷ��������ķ���Ƕ�..."<<endl;
    octave.computeAm();//����ǶȺ����
    
    
    
    test(image, octave);
    return;
    
    
    //����������
    
    //�ӳ߶�Ϊ0.5 ��ʼ����ͼ��Ľ�����
    GaussTwoDimen* g = new GaussTwoDimen[SIFTParam::S];
    Mat* temp = new Mat[SIFTParam::S];
    float kmi = 1.0;
    for(int i=0; i<SIFTParam::S; ++i)
    {
        temp[i] = image.clone();
        //��ʼ����˹����
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
    // //imwrite("img/Lena�Ķ�ά��˹ģ��_0.6.jpg", temp);
    // delete g;
}