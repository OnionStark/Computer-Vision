#include"base.h"
#include"Parymid.h"
#include"Sift.h"
#include"Gaussy.h"
#include"general.h"
const float PI = 3.1415926535898f;

template<typename T>
ImgGroup<T>::~ImgGroup()
{
    clear();
}

template<typename T>
void ImgGroup<T>::clear()
{
    if(img!=nullptr) 
    {
        delete[] img;
        img = nullptr;
    }
    if(keno != nullptr)
    {
        delete[]  keno;
        keno = nullptr;
    }
}
template<typename T>
void ImgGroup<T>::init()
{
    if(o ==-2) return;
    clear();
    img = new Mat[T::S];
    keno = new float[T::S];
    //�ѳ߶ȿռ���㶨��
    //��ʱ������
    if(o == -1)
    {
        keno[0] = 2.0*T::keno;
    }
    else
    {
        keno[0] = (1<<o)*T::keno;
    }
    
    for(int i=1; i<T::S; ++i)
        keno[i] = keno[i-1] * T::k;
}

template<typename T>
void ImgGroup<T>::working(GaussTwoDimen* g)
{
    for(int i=1; i< T::S; ++i)
    {
        img[i].create(img[0].rows, img[0].cols, CV_8UC(1));
        g[i-1].working(img[i], img[i-1]);
    }
}

//-----------DogGroup----------------

//-----------ImgOctave---------------
template<typename T>
ImgOctave<T>::ImgOctave(const Mat& img):data(nullptr), dog(nullptr)
{
    //��ʼ��
    init();
    //��ͼ������һ���õ� -1 ��ĵ�һ��ͼ    
    bilinear(data[0][0], img, 2.0);    
    GaussTwoDimen *g = new GaussTwoDimen[T::S-1];
    float kk = T::k*T::k;
    float k1 = kk, k2 = 1.0;
    for(int i=0;i<T::S-1; ++i)
    {
        g[i].init(2.0*sqrt(k1-k2)*T::keno);
        k1 *= kk;
        k2 *= kk;
    }
    
    data[0].working(g); 
    
    //��ʼ����������
    
    delete[] g;
    
    working();
    
    write();
}
template<typename T>
void ImgOctave<T>::init()
{
    //����O+2������� ���е�һ����
    data = new ImgGroup<T>[T::O+1];
    for(int i=0; i<=T::O; ++i)
    {
        data[i].setO(i-1);
        data[i].init();
    }
}

template<typename T>
ImgOctave<T>::~ImgOctave()
{
    if(data!=nullptr)   delete[] data;
    if(dog!=nullptr) delete[] dog;
}

void test()
{
    Mat t;
    ImgOctave<SIFTParam> i(t);
}

template<typename T>
void ImgOctave<T>::working()
{
    GaussTwoDimen* g= new GaussTwoDimen[T::S-1];
    float kk = T::k*T::k;
    float k1 = kk, k2 = 1.0;
    for(int i=0;i<T::S-1; ++i)
    {
        g[i].init(sqrt(k1-k2)*T::keno);
        k1 *= kk;
        k2 *= kk;
    }
    //�Ƚ��н������
    
    for(int i=1;i<=T::O; ++i)
    {
        //�ӵ�1�鿪ʼ
        downSampling(data[i][0], data[i-1][T::s]);
        data[i].working(g);
    }
    delete[] g;
}
template<typename T>
void ImgOctave<T>::write()
{
    return;
    //�� x ��� y �� �߶�_.jpg
    for(int i = -1; i<T::O; ++i)
    {
        for(int j=0;j<T::S; ++j)
            writeImg(i, j);
    }
}
template<typename T>
void ImgOctave<T>::writeImg(int x, int y)
{
    ostringstream haha;
    haha<<"img/��"<<x<<"���"<<y<<"�ų߶�_"<<data[x+1].getKeno(y)<<".jpg";
    imwrite(haha.str(), data[x+1][y]);
}

