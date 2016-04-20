#ifndef _PARYMID
#define _PARYMID
#include"base.h"
#include"Gaussy.h"
#include"general.h"

//因为坑爹的模板类的声明和定义不能分开，所以一个h文件这么长～～～～～～～～～～～～
extern const float PI;
//金字塔中一组的图像类
template<typename T>
class ImgGroup
{
    int o; //归属于哪一层
    float* keno; //每张图像的尺度
    Mat* img;//图像的数据
    
    public:
    ImgGroup():keno(nullptr), img(nullptr), o(-2) { }
    ImgGroup(int ino):keno(nullptr), img(nullptr) , o(ino) { init(); }
    virtual ~ImgGroup();
    void setO(int ino) {o = ino; }
    void init();
    void clear();
        
    void working(GaussTwoDimen* g);//为接下来的图像生成
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
        //keno 的 delete 职责留给ImgGroup
        //if(keno!=nullptr) delete[] keno;
        if(img!=nullptr) delete[] img;
    }
    
    float*& setKeno() { return keno; }
    
    Mat& operator [] (int index)
    {
        return img[index];
    }
};

//记录特征点的振幅和主方向
struct AmAndDirection
{
    float am[36]; //振幅直方图
    float direction;//方向，弧度值
    int index;//最大振幅的下标
    //辅助的方向
    int secondIndex[2]; //初始化为-1， 当不为-1的时候，secondDirection 才有效
    float secondDirection[2];
    
    AmAndDirection():index(-1)
    {
        secondIndex[0] = secondIndex[1] = -1;
        for(int i=0; i<36; ++i)
            am[i] = 0.0f;
    }
    //看是不是还有第二方向,只拿前两个
    void isSecondDirection()
    {
        return; //暂时没有辅助方向
    }
};

//记录极值点
struct ExtremePoint
{
    //记录极值点需要图像的坐标、dog 层数、dog第几张
    int x, y;//坐标
    int o;//层数 [0,O-1]
    int j;//第几张 [1,s]
    AmAndDirection amd;
    
    ExtremePoint(int inx, int iny, int ino, int inj):
        x(inx), y(iny), o(ino), j(inj)
        {}
    void getActuallyXY(int a[2])
    {
        //坐标的换算关系是 2^n*x+2^n-1
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

//金字塔中的层
//T 中包含建立金字塔的信息
template<typename T>
class ImgOctave
{
    ImgGroup<T>* data;// 金字塔的组
    DogGroup<T>* dog;//Dog 算子
    typedef vector<ExtremePoint> vPoint;
    vPoint point;//动态数组
    
    void init();
    void writeImg(int x, int y);
    void writeDog(int i, int j)
    {
        return;
        ostringstream haha;
        haha<<"img/Dog/第"<<i<<"组第"<<j<<"张-第"<<j-1<<"张.jpg";
        imwrite(haha.str(), dog[i-1][j-1]);
    }
    
    bool isFeature(ExtremePoint& dd)
    {
        //const int INT_MAX = 2147483647;
        const float img_scale = 1.f/(255.0);//图像归一化参数
        const float firstD = img_scale*0.5f;//一阶偏导的分母
        const float secondD = img_scale;//二阶偏导的分母
        const float secondCrossD = img_scale*0.25f;//二阶混合偏导的分母
        const float RR = (T::r+1)*(T::r+1)/T::r;
        //const float RR = 4.5f;
        
        int& r = dd.x;
        int& c = dd.y;
        int& j = dd.j;
        int i=0;
        float dxx, dyy, dkk, dxy, dxk, dyk; //继续二阶偏导和二阶混合偏导
        //求一阶偏导
        float dD[3];
        float X[3];
        float v2;//当前的像素值
        //SIFT_MAX_STEPS 是最高的迭代次数，为5
        for(; i< T::SIFT_MAX_STEPS; ++i)
        {
            const xjMat now( dog[dd.o][j] );
            const xjMat prev( dog[dd.o][j-1]);
            const xjMat next( dog[dd.o][j+1]);
            v2 =  static_cast<float>(now.get(r,c))*2.0f; //当前像素值的两倍
            //用于求二阶偏导

            dD[0] = (now.get( r+1, c) - now.get(r-1, c))*firstD;
            dD[1] = (now.get(r, c+1) - now.get(r, c+1))*firstD;
            dD[2] = (next.get(r,c) - prev.get(r, c) ) * firstD;
                      
            //x 表示行，y 表示列
            //二阶偏导
            dxx = (now.get(r+1, c)+ now.get(r-1, c) - v2)*secondD;
            dyy = (now.get(r, c+1) + now.get(r, c+1) - v2)*secondD;
            dkk = (next.get(r, c) + prev.get(r, c) - v2)*secondD;
            //二阶混合偏导
            dxy = (now.get(r-1,c-1) + now.get(r+1, c+1) - now.get(r-1, c+1) - now.get(r+1, c-1))*secondCrossD;
            dxk = (next.get(r+1, c) + prev.get(r-1, c) - next.get(r-1,c) - prev.get(r+1, c))*secondCrossD;
            dyk = (next.get(r, c+1) + prev.get(r, c-1) - next.get(r, c-1) - prev.get(r, c+1))*secondCrossD;
            
            
            //构造 ? 2 f / ?X 2 
            float H[3][3];
            H[0][0] = dxx;
            H[0][1] = H[1][0] = dxy;
            H[0][2] = H[2][0] = dxy;
            H[1][1] = dyy;
            H[1][2] = H[2][1] = dyk;
            H[2][2] = dkk;
            
            //求三个坐标的偏移量
            //X = H-1 x dD
            
            inverseMatrix(H);//求H-1, 结果放在它本身

            matrixDot(H, dD,X); //两个矩阵的乘法
            //由公式可以看得还差一个符号
            X[0] = -X[0];
            X[1] = -X[1];
            X[2] = -X[2];
            //求得各个维度的偏移量
            //[2][1][0] 分别是 k, y, x
            //如果三个维度的值都小于0.5 那么迭代停止，找到了特征点
            if( fabs(X[0]) < 0.5f && fabs(X[1]) <0.5f && fabs(X[2]) <0.5f)
                break; //找到特征点
            //如果三个维度的其中一个维度大于一个很大的数，返回
            if( fabs(X[0]) > float(INT_MAX/3) ||
                fabs(X[1]) > float(INT_MAX/3) ||
                fabs(X[2]) > float(INT_MAX/3) )
                return false;
            
            
            //由上面得到的偏移量重新定义插值中心的坐标位置
            r += cvRound(X[0]);
            c += cvRound(X[1]);
            j += cvRound(X[2]);
            
            
            
            
            //如果新的坐标超出了金字塔的范围，则说明该极值点不是特征点，返回
            //否则重新迭代
            if( j <1 || j> T::s || r< 0 || r>=now.rows
                || c<0 || c>= now.cols)     return false;
                
        }
        
        if( i >= T::SIFT_MAX_STEPS) return false;
        
        //计算特征点的响应值，如果绝对值小于经验值，那么抛弃
        float contr = v2*img_scale + dD[0]*X[0]*dD[1]*X[1]*dD[2]*X[2]*0.5f;
        
        //cout<<"响应值  "<<contr<<endl;
        if(contr*T::s < T::TT) return false;
        //去除边缘极值点
        
        //求x y 的二阶偏导，混合偏导
        float Tr = dxx+dyy;
        float Det = dxx*dyy - dxy*dxy;
        
        if( Det>0 && (Tr*Tr / Det) < RR) return true;
        else return false;
    }
            
    
    //@参数 i 是组数， j 是层数
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
                    //和自身的8领域比                 
                    
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
    
    //进行方向的插值
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
    
    //@参数，Keno 的长度为T::s
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
                //计算每个点的振幅
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
                //由于theta (-PI/2, PI/2) 的范围，所以还要变换
                if(y_1 <0) theta +=PI;
                else if(x_1<0 && y_1 >0) theta += PI*2.0f;                
                int index = theta/step;
                if(index == 36) --index;
                //高斯函数加权
                //重写高斯加权
                histogram[index] += ma*(*g)(m- p.x, n-p.y);
            }
        }
        
        //进行直方图的平滑
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
        
        //计算特征点的主振幅和方向
        int max = 0;
        for(int i=1; i<36; ++i)
        {
            if( p.amd.am[max] < p.amd.am[i]) max = i;
        }
        p.amd.index = max;
        
        //进行方向的插值
        p.amd.direction = getDirection(p, max);
        //看是否有第二方向
        p.amd.isSecondDirection();
    }
    
    public:
    ImgOctave(const Mat& image);
    virtual ~ImgOctave();
    void working();//从第0层开始构建金字塔

    void produceDog()
    {
        dog = new DogGroup<T>[T::O];//产生 o 层的dog算子
        //计算dog算子
        for(int i=1; i<= T::O; ++i)
        {
            for(int j=1; j<T::S; ++j)
            {
                //
                dog[i-1].setKeno() = data[i].getKenoArray();
                differ(dog[i-1][j-1], data[i][j], data[i][j-1]);
                
                //第i层的第j-1 张 - 第 j 张 dog
                writeDog(i, j);
            }
        }
    }
    
    //计算极值点，并且记录
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
    
    void write();//把高斯金字塔的图像写下去
    
    //计算特征点的振幅和方向
    void computeAm()
    {
        //计算某个圆域，还有高斯核
        //预处理高斯加权函数
        float Keno[T::s];//高斯核
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
        
        //计算该层的尺度
        //处理高斯函数
        for(int i=0;i<point.size(); ++i)
        {
            computeAmHelp(point[i], Keno[point[i].j-1] , g[point[i].j-1]);
        }
        
        for(int i=0;i<T::s; ++i)
            delete g[i];
        
        //有些特征点的振幅为0！！！
        //是不是出错了
        // for(int i=0;i<point.size(); ++i)
        // {
        //     cout<<point[i].amd.index<<"  "<<point[i].amd.am[ point[i].amd.index ]<<"  "<<point[i].amd.direction<<endl;
        // }
    }
};

void test();


#endif