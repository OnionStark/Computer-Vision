#include"general.h"
#include"Gaussy.h"
//如果image 为3通道图，则转化为灰度图
void converToGray(Mat& target, const Mat& image)
{
    target.create(image.rows, image.cols, CV_8UC(1));
    xjMat t(target);
    const xjMat source(image);
    for(int i = 0; i<image.rows; ++i)
    {
        for(int j=0;j<image.cols; ++j)
        {
            //Gray = (R*299 + G*587 + B*114 + 500) / 1000
            t.set(i, j) = (source.get(i, j, R)*299 + source.get(i, j, G)*587 + source.get(i, j, B)*114 + 500)/1000;
        }
    }
}

//双线性插值
void bilinear(Mat& target,  const Mat& image, float scale)
{
    int width = image.cols*scale, height = image.rows*scale;
    target.create(height, width, CV_8UC(1));
    xjMat temp(target);
    const xjMat source(image);
    for(int i=0;i<height; ++i)
    {
        int r = i/scale;
        float u = i/scale-r;
        for(int j=0;j<width; ++j)
        {
            int c = j/scale;
            float v = j/scale-c;
            temp.set(i, j) = (1-u)*(1-v)*source.get(r,c) + (1-u)*v*source.get(r, c+1) +
                u*(1-v)*source.get(r+1, c) +u*v*source.get(r+1, c+1);
        }
    }
}

//隔点降采样
void downSampling(Mat& target, const Mat& image)
{
    int width = image.cols>>1, height = image.rows>>1;
    target.create(height, width, CV_8UC(1));
    xjMat temp(target);
    const xjMat source(image);
    for(int i=0;i<height; ++i)
    {
        for(int j=0; j<width; ++j)
        {
            temp.set(i, j) = source.get( (i<<1) + 1, (j<<1) +1);
        }
    }
}
//计算两张图片的差值
void differ(Mat& target, const Mat&image1, const Mat& image2)
{
    target.create(image1.rows, image1.cols, CV_8UC(1));
    xjMat temp(target);
    
    const xjMat source1(image1);
    const xjMat source2(image2);
    for(int i=0; i<image1.rows; ++i)
    {
        for(int j=0;j<image1.cols; ++j)
        {
            auto t1 = source1.get(i, j), t2 = source2.get(i, j);
            if(t1>t2)
            {
                temp.set(i, j) = t1 - t2;
            }
            else temp.set(i, j) = 0;
        }
    }
}


void show(const string path, const Mat& im)
{
    namedWindow(path, WINDOW_AUTOSIZE);
    imshow(path, im);
    waitKey(0);
}

//三阶矩阵求逆
void inverseMatrix(float M[3][3])
{
    float det = M[0][0]*M[1][1]*M[2][2] + M[0][1]*M[1][2]*M[2][0]+
        M[0][2]*M[1][0]*M[2][1] - M[0][0]*M[1][2]*M[2][1] - 
        M[0][1]*M[1][0]*M[2][2] - M[0][2]*M[1][1]*M[2][0];
    if(det == 0.0f) return;
    
    float temp[3][3];
    temp[0][0] = M[1][1]*M[2][2] - M[2][1]*M[1][2];
    temp[0][1] = M[2][1]*M[0][2] - M[0][1]*M[2][2];
    temp[0][2] = M[0][1]*M[1][2] - M[0][2]*M[1][1];
    temp[1][0] = M[1][2]*M[2][0] - M[2][2]*M[1][0];
    temp[1][1] = M[2][2]*M[0][0] - M[0][2]*M[2][0];
    temp[1][2] = M[0][2]*M[1][0] - M[0][0]*M[1][2];
    temp[2][0] = M[1][0]*M[2][1] - M[2][0]*M[1][1];
    temp[2][1] = M[2][0]*M[0][1] - M[0][0]*M[2][1];
    temp[2][2] = M[0][0]*M[1][1] - M[0][1]*M[1][0];
    
    for(int i=0; i<3; ++i)
    {
        for(int j=0;j<3; ++j)
        {
            M[i][j] = temp[i][j]/det;
        }
    }
}


void matrixDot(float M[3][3], float dD[3], float X[3])
{
    for(int i=0; i<3; ++i)
    {
        X[i] = 0.0f;
        for(int j=0; j<3; ++j)
        {
            X[i] += M[i][j]*dD[j];
        }
    }
}

