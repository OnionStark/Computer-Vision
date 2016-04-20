#include"IntelligentScissors.h"
#include"general.h"
#include"XjMat.h"
#include"List.h"
const float PI = 3.1415926535898f;
const float PI_1 = 1.0f / PI;
const float sqrt2 = 1.414213562f;
float* Compare::g = nullptr;


void intellSci(Mat& img)
{
	//先对图片进行处理，如果不是灰度图片就转为灰度图片
	Mat target = img;
	if (img.channels() != 1)
	{
		converToGray(target, img);
	}
	IntellSci sci(target);
	sci.init();

	IplImage ipl_image = target;

	ActiveParam active = { &sci, &ipl_image };
	//寻路算法
	const char* name = "working";
	namedWindow(name, WINDOW_AUTOSIZE);
	setMouseCallback(name, activePrint, &active);
	cvShowImage(name, &ipl_image);
	waitKey();
}

//鼠标的回调函数，用来绘制线条
void activePrint(int m_event, int x, int y, int flags, void* param)
{

	const char* name = "working";
	ActiveParam* active = reinterpret_cast<ActiveParam*>(param);
	IplImage* imgptr = active->image;

	switch (m_event)
	{
	case CV_EVENT_MOUSEMOVE:
	{
		//if (flags == 1)
		//{
		//	imgptr->imageData[y*imgptr->width + x] = 255;
		//	cvShowImage(name, imgptr);
		//}
		break;
	}
	case CV_EVENT_LBUTTONDOWN:
	{
		imgptr->imageData[y*imgptr->width + x] = 255;
		DP(x, y, active->ins);
		
		cvShowImage(name, imgptr);

		break;
	}
	default:
		break;
	}
}


//寻路函数
void DP(int sx, int sy, IntellSci* ins)
{
	cout << "seed point " << sx << "  " << sy << endl;
	int cols = ins->img.cols;
	int index = sy*cols + sx;
	float* g = new float[ins->img.rows*ins->img.cols];
	bool* e = new bool[ins->img.rows*ins->img.cols]{};
	int Noffset[8];
	Noffset[0] = -cols - 1;
	Noffset[1] = -cols;
	Noffset[2] = 1-cols;
	Noffset[3] = -1;
	Noffset[4] = 1;
	Noffset[5] = cols-1;
	Noffset[6] = cols;
	Noffset[7] = cols+1;

	LList<int, Compare> list;
	Compare::g = g;
	g[index] = 0.0f;
	list.insert(index);
	int last = -1;
	while (!list.empty())
	{
		int q = list.first();
		list.removeFirst();
		//cout << q%cols << "  " << q / cols << "  " << list.size() << endl;
		e[q] = true;
		for (int i = 0; i < 8; ++i)
		{
			int r = q + Noffset[i];
			if (r<0 || r>=ins->img.rows*ins->img.cols || e[r]) continue;
			float gtemp = g[q] + ins->ILC(q%cols, q / cols, r%cols, r / cols);
			//cout << r%cols << "  " << r / cols << "  " << gtemp;
			if (!list.isContain(r))
			{
				g[r] = gtemp;
				ins->p[r] = q;
				last = r;
				list.insert(r);
			}
			else if ( gtemp < g[r])
			{
				//cout << "  remove";
				list.remove(r);
			}
			//cout << endl;
		}
	}

	while (ins->p[last] != index)
	{
		ins->img.data[last] = 255;
		last = ins->p[last];
	}

}




//-----------IntellSci---------
float IntellSci::Wz = 0.3f;
float IntellSci::Wd= 0.1f;
float IntellSci::Wg = 0.3f;
float IntellSci::Wp = 0.1f;
float IntellSci::Wi = 0.1f;
float IntellSci::Wo = 0.1f;

void IntellSci::init()
{
	//channls == 1
	if (Fz != nullptr)
		delete[] Fz;
	Fz = new uchar[img.rows*img.cols];
	if (Fg != nullptr)
		delete[] Fg;
	Fg = new float[img.rows*img.cols];
	if (Dp != nullptr)
		delete[] Dp;
	Dp = new Axis<short>[img.rows*img.cols];
	if (p != nullptr)
		delete[] p;
	p = new int[img.rows*img.cols];


	//计算Fz
	computeFz();
	//计算Fg
	computeFgDp();
}

IntellSci::~IntellSci()
{
	if (Fz != nullptr) delete[] Fz;
	if (Fg != nullptr) delete[] Fg;
	if (Dp != nullptr) delete[] Dp;
}

void IntellSci::computeFz()
{
	xjMat temp(img);
	for (int i = 0; i < temp.rows; ++i)
	{
		for (int j = 0; j < temp.cols; ++j)
		{
			Fz[i*temp.cols + j] = temp.get(i - 1, j) + temp.get(i + 1, j) + temp.get(i, j + 1) + temp.get(i, j - 1)
				- 4 * temp.get(i, j) == 0 ? 0 : 1;

		}
	}
}

void IntellSci::computeFgDp()
{
	xjMat temp(img);
	float max = 0.0f;
	for (int i = 0; i < temp.rows; ++i)
	{
		for (int j = 0; j < temp.cols; ++j)
		{
			int a = i*temp.cols + j;
			Dp[a][0] = temp.get(i, j + 1) - temp.get(i, j);//Iy
			Dp[a][1] = temp.get(i, j) - temp.get(i + 1, j);//-Ix
			Fg[a] = Dp[a].getM();
			//归一化Dp
			Dp[a][0] /= Fg[a];
			Dp[a][1] /= Fg[a];
			if (Fg[a] > max) max = Fg[a];
		}
	}

	//
	for (int i = 0; i < temp.rows; ++i)
	{
		for (int j = 0; j < temp.cols; ++j)
		{
			int index = i*temp.cols + j;
			Fg[index] = 1 - Fg[index] / max;
		}
	}
}

float IntellSci::computeFd(const Axis<short>& p, const Axis<short>& q)
{
	return computeFd(p.x, p.y, q.x, q.y);
}

float IntellSci::computeFd(int px, int py, int qx, int qy)
{
	int pindex = py*img.cols + px;
	int qindex = qy*img.cols + qx;
	//q-p
	Axis<int> qp(qx - px, qy - py);
	//L
	Axis<float> L;
	if (dot(qp, Dp[pindex]) >= 0)
	{
		L.x = qp.x / qp.getM();
		L.y = qp.y / qp.getM();
	}
	else
	{
		L.x = -qp.x / qp.getM();
		L.y = -qp.y / qp.getM();
	}

	float dp, dq;
	dp = dot(L, Dp[pindex]);
	dq = dot(L, Dp[qindex]);

	return (2.0f / 3.0f*PI_1*(acos(dp) + acos(dq)));
}

float IntellSci::ILC(int px, int py, int qx, int qy)
{
	//fi,fo
	int ki = 1, kj = 1;
	int index = py*img.cols + px;
	int qindex = qy*img.cols + qx;
	if (Dp[index][1] > 0) ki = -1;
	if (Dp[index][0] < 0) kj = -1;
	xjMat temp(img);
	float fi = temp.get(px + ki, py + kj) / 255.0f;
	float fo = temp.get(px - ki, py - kj) / 255.0f;
	float fp = temp.get(px, py) / 255.0f;
	float fg = Fg[qindex];
	if (qx == px || qy == py) fg /= sqrt2;
	return (Wz*Fz[qindex] + Wg*fg + Wd*computeFd(px, py, qx, qy) + Wp*fp +
		Wi*fi + Wo*fo);
}
float IntellSci::ILC(const Axis<int>& p, const Axis<int>& q)
{
	return ILC(p.x, p.y, q.x, q.y);
}