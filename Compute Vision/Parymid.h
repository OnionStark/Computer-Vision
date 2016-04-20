#ifndef _PARYMID
#define _PARYMID
#include"base.h"
#include"Gaussy.h"
#include"general.h"

//��Ϊ�ӵ���ģ����������Ͷ��岻�ֿܷ�������һ��h�ļ���ô��������������������������
extern const float PI;
//��������һ���ͼ����
template<typename T>
class ImgGroup
{
    int o; //��������һ��
    float* keno; //ÿ��ͼ��ĳ߶�
    Mat* img;//ͼ�������
    
    public:
    ImgGroup():keno(nullptr), img(nullptr), o(-2) { }
    ImgGroup(int ino):keno(nullptr), img(nullptr) , o(ino) { init(); }
    virtual ~ImgGroup();
    void setO(int ino) {o = ino; }
    void init();
    void clear();
        
    void working(GaussTwoDimen* g);//Ϊ��������ͼ������
    float getKeno(int j) const { return keno[j]; }
    float* getKenoArray() const { return keno; }
    Mat& operator [] (int index)
    {
        return img[index];
    }
};

template<typename T>
class DogGroup
{
    float *keno;
    Mat* img;
    
    void init()
    {
        int length  = T::s+2;
        img = new Mat[length];
    }
    public:
    DogGroup(): keno(nullptr), img(nullptr) { init(); }
    virtual ~DogGroup()
    {
        //keno �� delete ְ������ImgGroup
        //if(keno!=nullptr) delete[] keno;
        if(img!=nullptr) delete[] img;
    }
    
    float*& setKeno() { return keno; }
    
    Mat& operator [] (int index)
    {
        return img[index];
    }
};

//��¼������������������
struct AmAndDirection
{
    float am[36]; //���ֱ��ͼ
    float direction;//���򣬻���ֵ
    int index;//���������±�
    //�����ķ���
    int secondIndex[2]; //��ʼ��Ϊ-1�� ����Ϊ-1��ʱ��secondDirection ����Ч
    float secondDirection[2];
    
    AmAndDirection():index(-1)
    {
        secondIndex[0] = secondIndex[1] = -1;
        for(int i=0; i<36; ++i)
            am[i] = 0.0f;
    }
    //���ǲ��ǻ��еڶ�����,ֻ��ǰ����
    void isSecondDirection()
    {
        return; //��ʱû�и�������
    }
};

//��¼��ֵ��
struct ExtremePoint
{
    //��¼��ֵ����Ҫͼ������ꡢdog ������dog�ڼ���
    int x, y;//����
    int o;//���� [0,O-1]
    int j;//�ڼ��� [1,s]
    AmAndDirection amd;
    
    ExtremePoint(int inx, int iny, int ino, int inj):
        x(inx), y(iny), o(ino), j(inj)
        {}
    void getActuallyXY(int a[2])
    {
        //����Ļ����ϵ�� 2^n*x+2^n-1
        a[0] = ((x+1)<<o) - 1;
        a[1] = ((y+1)<<o) - 1;
    }
    float getAm()
    {
        return amd.am[ amd.index ]; 
    }
    float getDirection()
    {
        return amd.direction;
    }
};

//�������еĲ�
//T �а�����������������Ϣ
template<typename T>
class ImgOctave
{
    ImgGroup<T>* data;// ����������
    DogGroup<T>* dog;//Dog ����
    typedef vector<ExtremePoint> vPoint;
    vPoint point;//��̬����
    
    void init();
    void writeImg(int x, int y);
    void writeDog(int i, int j)
    {
        return;
        ostringstream haha;
        haha<<"img/Dog/��"<<i<<"���"<<j<<"��-��"<<j-1<<"��.jpg";
        imwrite(haha.str(), dog[i-1][j-1]);
    }
    
    bool isFeature(ExtremePoint& dd)
    {
        //const int INT_MAX = 2147483647;
        const float img_scale = 1.f/(255.0);//ͼ���һ������
        const float firstD = img_scale*0.5f;//һ��ƫ���ķ�ĸ
        const float secondD = img_scale;//����ƫ���ķ�ĸ
        const float secondCrossD = img_scale*0.25f;//���׻��ƫ���ķ�ĸ
        const float RR = (T::r+1)*(T::r+1)/T::r;
        //const float RR = 4.5f;
        
        int& r = dd.x;
        int& c = dd.y;
        int& j = dd.j;
        int i=0;
        float dxx, dyy, dkk, dxy, dxk, dyk; //��������ƫ���Ͷ��׻��ƫ��
        //��һ��ƫ��
        float dD[3];
        float X[3];
        float v2;//��ǰ������ֵ
        //SIFT_MAX_STEPS ����ߵĵ���������Ϊ5
        for(; i< T::SIFT_MAX_STEPS; ++i)
        {
            const xjMat now( dog[dd.o][j] );
            const xjMat prev( dog[dd.o][j-1]);
            const xjMat next( dog[dd.o][j+1]);
            v2 =  static_cast<float>(now.get(r,c))*2.0f; //��ǰ����ֵ������
            //���������ƫ��

            dD[0] = (now.get( r+1, c) - now.get(r-1, c))*firstD;
            dD[1] = (now.get(r, c+1) - now.get(r, c+1))*firstD;
            dD[2] = (next.get(r,c) - prev.get(r, c) ) * firstD;
                      
            //x ��ʾ�У�y ��ʾ��
            //����ƫ��
            dxx = (now.get(r+1, c)+ now.get(r-1, c) - v2)*secondD;
            dyy = (now.get(r, c+1) + now.get(r, c+1) - v2)*secondD;
            dkk = (next.get(r, c) + prev.get(r, c) - v2)*secondD;
            //���׻��ƫ��
            dxy = (now.get(r-1,c-1) + now.get(r+1, c+1) - now.get(r-1, c+1) - now.get(r+1, c-1))*secondCrossD;
            dxk = (next.get(r+1, c) + prev.get(r-1, c) - next.get(r-1,c) - prev.get(r+1, c))*secondCrossD;
            dyk = (next.get(r, c+1) + prev.get(r, c-1) - next.get(r, c-1) - prev.get(r, c+1))*secondCrossD;
            
            
            //���� ? 2 f / ?X 2 
            float H[3][3];
            H[0][0] = dxx;
            H[0][1] = H[1][0] = dxy;
            H[0][2] = H[2][0] = dxy;
            H[1][1] = dyy;
            H[1][2] = H[2][1] = dyk;
            H[2][2] = dkk;
            
            //�����������ƫ����
            //X = H-1 x dD
            
            inverseMatrix(H);//��H-1, �������������

            matrixDot(H, dD,X); //��������ĳ˷�
            //�ɹ�ʽ���Կ��û���һ������
            X[0] = -X[0];
            X[1] = -X[1];
            X[2] = -X[2];
            //��ø���ά�ȵ�ƫ����
            //[2][1][0] �ֱ��� k, y, x
            //�������ά�ȵ�ֵ��С��0.5 ��ô����ֹͣ���ҵ���������
            if( fabs(X[0]) < 0.5f && fabs(X[1]) <0.5f && fabs(X[2]) <0.5f)
                break; //�ҵ�������
            //�������ά�ȵ�����һ��ά�ȴ���һ���ܴ����������
            if( fabs(X[0]) > float(INT_MAX/3) ||
                fabs(X[1]) > float(INT_MAX/3) ||
                fabs(X[2]) > float(INT_MAX/3) )
                return false;
            
            
            //������õ���ƫ�������¶����ֵ���ĵ�����λ��
            r += cvRound(X[0]);
            c += cvRound(X[1]);
            j += cvRound(X[2]);
            
            
            
            
            //����µ����곬���˽������ķ�Χ����˵���ü�ֵ�㲻�������㣬����
            //�������µ���
            if( j <1 || j> T::s || r< 0 || r>=now.rows
                || c<0 || c>= now.cols)     return false;
                
        }
        
        if( i >= T::SIFT_MAX_STEPS) return false;
        
        //�������������Ӧֵ���������ֵС�ھ���ֵ����ô����
        float contr = v2*img_scale + dD[0]*X[0]*dD[1]*X[1]*dD[2]*X[2]*0.5f;
        
        //cout<<"��Ӧֵ  "<<contr<<endl;
        if(contr*T::s < T::TT) return false;
        //ȥ����Ե��ֵ��
        
        //��x y �Ķ���ƫ�������ƫ��
        float Tr = dxx+dyy;
        float Det = dxx*dyy - dxy*dxy;
        
        if( Det>0 && (Tr*Tr / Det) < RR) return true;
        else return false;
    }
            
    
    //@���� i �������� j �ǲ���
    void computeHelp(int i, int j)
    {
        xjMat temp(dog[i][j]);
        xjMat pre(dog[i][j-1]);
        xjMat next(dog[i][j+1]);
        char result = -2;//0 -1 1
        for(int x=0; x<dog[i][j].rows; ++x)
        {
            for(int y=0; y<dog[i][j].cols; ++y)
            {                
                auto f = [&]()->bool
                {
                    auto now = temp.get(x, y);
                    int cha = now - temp.get(x-1, y-1);
                    //�������8�����                 
                    
                    for(int m = x-1; m<=x+1; ++m)
                    {
                        for( int n = y-1; n<=y+1; ++n)
                        {
                            auto am = temp.get(m, n), bm = pre.get(m,n), cm = next.get(m, n);
                            if( ((cha*(now-bm))<=0) || ((cha*(now-cm))<=0) || ( (m!=x || n!=y) &&( (cha*(now-am)) <=0 ) ) ) 
                                return false;
                        }
                    }
                    return true;
                };

                if( f() )
                {
                    ExtremePoint temp(x, y, i, j);
                    if(isFeature(temp) )
                        point.push_back( temp );
                }
                
            }
        }
    }
    
    //���з���Ĳ�ֵ
    float getDirection(ExtremePoint& p, int i)
    {
        const float step = PI/18.0f;
        
        int a = i-1, b = i+1;
        if(i==0) a = 35;
        else if(i==35) b = 0;
        float B;
        float ttt = (2.0f*(p.amd.am[a]+p.amd.am[b] - 2.0f*p.amd.am[i]));
        if(ttt == 0.0f) return 0.0f;
        B = i+( p.amd.am[a] - p.amd.am[b])/ttt;
        B = PI+PI - step*B;
        if(B<0) B += PI+PI;
        else if(B>=PI+PI) B-=PI-PI;
        return B;
    }
    
    //@������Keno �ĳ���ΪT::s
    void computeAmHelp(ExtremePoint& p, float keno, gaussyTwo* g)
    {
        const float step = PI/18.0f;
        int radio = 4.5f*keno;
        int distance = 4.5f*keno;
        //float histogram[36] = {0.0f};
        float *histogram = p.amd.am;
        xjMat img( data[p.o][p.j] );
        for(int m = p.x-radio; m<= p.x+radio; ++m)
        {
            for(int n = p.y-radio; n<= p.y+radio; ++n)
            {
                //����ÿ��������
                if( (m-p.x)*(m-p.x) + (n-p.y)*(n-p.y) > distance ) continue;
                int x_1 = img.get(p.x + 1, p.y) - img.get(p.x-1, p.y);
                int y_1 = img.get(p.x, p.y+1) - img.get(p.x, p.y-1);
                
                float ma = sqrt( x_1*x_1 + y_1*y_1 );
                float theta;
                if(y_1 == 0)
                {
                    if(x_1 >0)  theta = PI*0.5f;
                    else if(x_1<0) theta = PI*1.5f;
                    else theta = 0.0f;
                }
                else theta = atan2( x_1, y_1 );
                //����theta (-PI/2, PI/2) �ķ�Χ�����Ի�Ҫ�任
                if(y_1 <0) theta +=PI;
                else if(x_1<0 && y_1 >0) theta += PI*2.0f;                
                int index = theta/step;
                if(index == 36) --index;
                //��˹������Ȩ
                //��д��˹��Ȩ
                histogram[index] += ma*(*g)(m- p.x, n-p.y);
            }
        }
        
        //����ֱ��ͼ��ƽ��
        // const float chengshu = 1.0f/16.0f;
        // 
        // p.amd.am[0] = ( histogram[34] + histogram[2] +4.0f*(histogram[35]+histogram[1]) + 6.0f*histogram[0])*chengshu;
        // p.amd.am[1] = ( histogram[35] + histogram[3] + 4.0f*(histogram[0]+histogram[2]) + 6.0f*histogram[1])*chengshu;
        // p.amd.am[35] = (histogram[33]+histogram[1] + 4.0f*(histogram[34] + histogram[0]) + 6.0f*histogram[35])*chengshu;
        // p.amd.am[34] = (histogram[32] + histogram[0] + 4.0f*(histogram[33] + histogram[35]) + 6.0f*histogram[34]) *chengshu;
        // for(int i = 2; i<=34; ++i)
        // {
        //     p.amd.am[i] = (histogram[i-2] + histogram[i+2] + 4.0f*(histogram[i-1]+histogram[i+1]) + 6.0f*histogram[i])*chengshu;
        // }
        
        //�����������������ͷ���
        int max = 0;
        for(int i=1; i<36; ++i)
        {
            if( p.amd.am[max] < p.amd.am[i]) max = i;
        }
        p.amd.index = max;
        
        //���з���Ĳ�ֵ
        p.amd.direction = getDirection(p, max);
        //���Ƿ��еڶ�����
        p.amd.isSecondDirection();
    }
    
    public:
    ImgOctave(const Mat& image);
    virtual ~ImgOctave();
    void working();//�ӵ�0�㿪ʼ����������

    void produceDog()
    {
        dog = new DogGroup<T>[T::O];//���� o ���dog����
        //����dog����
        for(int i=1; i<= T::O; ++i)
        {
            for(int j=1; j<T::S; ++j)
            {
                //
                dog[i-1].setKeno() = data[i].getKenoArray();
                differ(dog[i-1][j-1], data[i][j], data[i][j-1]);
                
                //��i��ĵ�j-1 �� - �� j �� dog
                writeDog(i, j);
            }
        }
    }
    
    //���㼫ֵ�㣬���Ҽ�¼
    void computePoint()
    {
        for(int i=0; i<T::O; ++i)
        {
            for(int j=1; j<=T::s; ++j)
            {
                computeHelp(i ,j);
            }
        }
    }
    
    vPoint& getExtremePoint()
    {
        return point;
    }
    
    void write();//�Ѹ�˹��������ͼ��д��ȥ
    
    //���������������ͷ���
    void computeAm()
    {
        //����ĳ��Բ�򣬻��и�˹��
        //Ԥ�����˹��Ȩ����
        float Keno[T::s];//��˹��
        gaussyTwo* g[T::s];
        Keno[0] = T::k*T::k*T::keno;
        for(int i=1;i<T::s; ++i)
        {
            Keno[i] = Keno[i-1]*T::k;
        }
        for(int i=0;i<T::s; ++i)
        {
            int radio = 4.5f*Keno[i];
            radio += radio+1;
            g[i] = new gaussyTwo(1.5f*Keno[i], radio, radio);
        }
        
        //����ò�ĳ߶�
        //�����˹����
        for(int i=0;i<point.size(); ++i)
        {
            computeAmHelp(point[i], Keno[point[i].j-1] , g[point[i].j-1]);
        }
        
        for(int i=0;i<T::s; ++i)
            delete g[i];
        
        //��Щ����������Ϊ0������
        //�ǲ��ǳ�����
        // for(int i=0;i<point.size(); ++i)
        // {
        //     cout<<point[i].amd.index<<"  "<<point[i].amd.am[ point[i].amd.index ]<<"  "<<point[i].amd.direction<<endl;
        // }
    }
};

void test();


#endif