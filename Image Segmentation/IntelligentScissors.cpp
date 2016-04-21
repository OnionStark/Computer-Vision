#include"IntelligentScissors.h"
#include"general.h"
#include"XjMat.h"
#include"List.h"
#include"GlobalClass.h"
const float PI = 3.1415926535898f;
const float PI_1 = 1.0f / PI;
const float sqrt2 = 1.414213562f;
float* Compare::g = nullptr;
bool isshow = true;

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


	IplImage ipl_image = img;

	ActiveParam active = { &sci, &ipl_image };
	//寻路算法
	const char* name = "working";
	namedWindow(name, WINDOW_AUTOSIZE);
	setMouseCallback(name, activePrint, &active);
	cvShowImage(name, &ipl_image);
	waitKey();
}

//鼠标的回调函数，用来绘制线条
//void activePrint(int m_event, int x, int y, int flags, void* param)
//{
//	const char* name = "working";
//	ActiveParam* active = reinterpret_cast<ActiveParam*>(param);
//	IplImage* imgptr = active->image;
//
//	switch (m_event)
//	{
//	case CV_EVENT_MOUSEMOVE:
//	{
//		return;
//		if (getGlobal().isFirst)
//		{
//			showinImg(imgptr, active->ins, y*imgptr->width + x,
//				getGlobal().py*imgptr->width + getGlobal().px);
//			cvShowImage(name, imgptr);
//		}
//
//		break;
//	}
//	case CV_EVENT_LBUTTONDOWN:
//	{
//		imgptr->imageData[y*imgptr->width * 3 + x * 3] = 0;//R
//		imgptr->imageData[y*imgptr->width * 3 + x * 3 + 1] =0;//R
//		imgptr->imageData[y*imgptr->width * 3 + x * 3 + 2] = 255;//R
//
//		cout << "选定点 " << x << "  " <<y<< endl;
//		if (!getGlobal().isFirst)
//		{
//			getGlobal().px = x, getGlobal().py = y;
//			cout << "正在预处理，请稍后。。。" << endl;
//			DP(getGlobal().px, getGlobal().py,active->ins);
//			getGlobal().isFirst = true;//首次计算所有的路径
//			getGlobal().setImage(imgptr);
//			cout << "预处理结束！" << endl;
//		}
//		else
//		{
//			getGlobal().updateImg(x, y, active->ins);
//			//cvReleaseImage(&imgptr);
//			imgptr = cvCloneImage(getGlobal().imgptr);
//		}
//		cvShowImage(name, imgptr);
//		break;
//	}
//	default:
//		break;
//	}
//}
void activePrint(int m_event, int x, int y, int flags, void* param)
{
	const char* name = "working";
	ActiveParam* active = reinterpret_cast<ActiveParam*>(param);
	IplImage* imgptr = active->image;

	switch (m_event)
	{
	case CV_EVENT_MOUSEMOVE:
	{
		//return;
		if (getGlobal().isFirst)
		{
			showinImg(imgptr, active->ins, y*imgptr->width + x,
				getGlobal().py*imgptr->width + getGlobal().px);
			cvShowImage(name, imgptr);
		}

		break;
	}
	case CV_EVENT_LBUTTONDOWN:
	{
		imgptr->imageData[y*imgptr->width * 3 + x * 3] = 0;//R
		imgptr->imageData[y*imgptr->width * 3 + x * 3 + 1] =0;//R
		imgptr->imageData[y*imgptr->width * 3 + x * 3 + 2] = 255;//R

		cout << "选定点 " << x << "  " <<y<< endl;
		if (!getGlobal().isFirst)
		{
			getGlobal().px = x, getGlobal().py = y;
			cout << "正在预处理，请稍后。。。" << endl;
			DP(getGlobal().px, getGlobal().py,active->ins);
			getGlobal().isFirst = true;//首次计算所有的路径
			getGlobal().setImage(imgptr);
			cout << "预处理结束！" << endl;
		}
		else
		{
			getGlobal().updateImg(x, y, active->ins);
			//cvReleaseImage(&imgptr);
			//imgptr = cvCloneImage(getGlobal().imgptr);
			cloneImage(imgptr, getGlobal().imgptr);

			cout << "正在重新定位点，请稍后。。。" << endl;
			DP(x, y, active->ins); //重新计算种子点
			cout << "结束，请继续选择下一个种子点" << endl;
		}
		cvShowImage(name, imgptr);
		break;
	}
	default:
		break;
	}
}
void DP(int sx, int sy, IntellSci* ins)
{
	int cols = ins->img.cols;
	int index = sy*cols + sx;

	float* g = new float[ins->img.rows*ins->img.cols];
	bool* e = new bool[ins->img.rows*ins->img.cols]{};
	int Noffset[8];
	Noffset[0] = -cols - 1;
	Noffset[1] = -cols;
	Noffset[2] = 1 - cols;
	Noffset[3] = -1;
	Noffset[4] = 1;
	Noffset[5] = cols - 1;
	Noffset[6] = cols;
	Noffset[7] = cols + 1;

	LList<int, Compare> list;
	Compare::g = g;
	g[index] = 0.0f;
	list.insert(index);
	while (!list.empty())
	{
		int q = list.first();
		list.removeFirst();
		e[q] = true;

		for (int i = 0; i < 8; ++i)
		{
			int r = q + Noffset[i];
			if (r<0 || r >= ins->img.rows*ins->img.cols || e[r]) continue;
			int x = q%cols, y = q / cols;

			float gtemp = g[q] + ins->ilc[y*cols * 8 + x * 8 + i];

			if (list.isContain(r) && gtemp < g[r])
			{
				list.remove(r);
			}
			if (!list.isContain(r))
			{
				g[r] = gtemp;
				ins->p[r] = q;
				list.insert(r);
			}
		}
	}
	delete[]g;
	delete[]e;
}

void showinImg(IplImage*&imgptr, IntellSci* ins, int free, int seed)
{
	if (free == seed) return;

	//imgptr = cvCloneImage(getGlobal().imgptr); //每次都复制
	cloneImage(imgptr, getGlobal().imgptr);

	int x, y;
	int cols = ins->img.cols;
	while (ins->p[free] != seed)
	{
		x = free%cols, y = free / cols;
		imgptr->imageData[y*cols * 3 + x * 3] = 0;
		imgptr->imageData[y*cols * 3 + x * 3+1] = 0;
		imgptr->imageData[y*cols * 3 + x * 3+2] = 255;
		free = ins->p[free];
	}
}


//-----------IntellSci---------
float IntellSci::Wz = 0.43f;
float IntellSci::Wd= 0.43f;
float IntellSci::Wg = 0.14f;
float IntellSci::Wp = 0.f;
float IntellSci::Wi = 0.f;
float IntellSci::Wo = 0.f;

void IntellSci::init()
{
	if (method == 1)
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

	}
	if (p != nullptr)
		delete[] p;
	p = new int[img.rows*img.cols];
	if (ilc != nullptr)
		delete[] ilc;
	ilc = new float[img.rows*img.cols * 8];

	//计算Fz
	if (method == 1)
	{
		computeFz();
		//计算Fg
		computeFgDp();
	}
	computeILC();
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
			if (temp.get(i - 1, j) + temp.get(i + 1, j) + temp.get(i, j + 1) + temp.get(i, j - 1)
				- 4 * temp.get(i, j) == 0)
				Fz[i*temp.cols + j] = 0;
			else Fz[i*temp.cols + j] = 1;
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
			Dp[a][0] = temp.get(i + 1, j) - temp.get(i, j);//Iy
			Dp[a][1] = temp.get(i, j) - temp.get(i, j + 1);//-Ix
			Fg[a] = Dp[a].getM();
			//归一化Dp
			Dp[a][0] /= Fg[a];
			Dp[a][1] /= Fg[a];
			if (Fg[a] > max) max = Fg[a];
		}
	}

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

	return (/*2.0f / 3.0f**/PI_1*(acos(dp) + acos(dq)));
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
	float fi = temp.get(py + kj, px + ki) / 255.0f;
	float fo = temp.get(py - kj, px - ki) / 255.0f;
	float fp = temp.get(py, px) / 255.0f;

	float fg = Fg[qindex];
	if (qx == px || qy == py) fg /= sqrt2;
	return (Wz*Fz[qindex] + Wg*fg + Wd*computeFd(px, py, qx, qy) + Wp*fp +
		Wi*fi + Wo*fo);
}
float IntellSci::ILC(const Axis<int>& p, const Axis<int>& q)
{
	return ILC(p.x, p.y, q.x, q.y);
}

void IntellSci::computeILC()
{
	if (method == 1)
	{
		forMethod1();
	}
	else
	{
		forMethod2();
	}

}

void IntellSci::forMethod1()
{
	int cols = img.cols;
	int Noffset[8];
	Noffset[0] = -cols - 1;
	Noffset[1] = -cols;
	Noffset[2] = 1 - cols;
	Noffset[3] = -1;
	Noffset[4] = 1;
	Noffset[5] = cols - 1;
	Noffset[6] = cols;
	Noffset[7] = cols + 1;


	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			for (int k = 0; k < 8; ++k)
			{
				int index = i*cols * 8 + j * 8 + k;
				int p = i*cols + j + Noffset[k];
				if (p <0 || p >= img.rows*img.cols)
					continue;
				ilc[index] = ILC(j, i, p%cols, p / cols);
			}
		}
	}
}

void IntellSci::forMethod2()
{
	int cols = img.cols;
	int Noffset[8];
	Noffset[0] = -cols - 1;
	Noffset[1] = -cols;
	Noffset[2] = 1 - cols;
	Noffset[3] = -1;
	Noffset[4] = 1;
	Noffset[5] = cols - 1;
	Noffset[6] = cols;
	Noffset[7] = cols + 1;

	xjMat temp(img);
	float max = 0.0f;
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			int index = i*cols*8+j*8;
			int k = 0;
			ilc[index + k++] = abs(temp.get(i - 1, j) - temp.get(i, j - 1)) / sqrt2;//link3
			ilc[index + k++] = abs(temp.get(i - 1, j - 1) + temp.get(i, j - 1) - temp.get(i - 1, j + 1)
				- temp.get(i, j + 1)) / 4;//link2
			ilc[index + k++] = abs(temp.get(i - 1, j) - temp.get(i, j + 1)) / sqrt2;//link1
			ilc[index + k++] = abs(temp.get(i - 1, j - 1) + temp.get(i - 1, j) - temp.get(i + 1, j - 1) - temp.get(i + 1, j)) / 4;//link5
			ilc[index + k++] = abs(temp.get(i - 1, j) + temp.get(i - 1, j + 1) - temp.get(i + 1, j) - temp.get(i + 1, j + 1)) / 4;//link0
			ilc[index + k++] = abs(temp.get(i, j - 1) - temp.get(i + 1, j)) / sqrt2;//link5
			ilc[index + k++] = abs(temp.get(i, j - 1) + temp.get(i + 1, j - 1) - temp.get(i, j + 1) - temp.get(i + 1, j + 1)) / 4;//link6
			ilc[index + k++] = abs(temp.get(i, j + 1) - temp.get(i + 1, j)) / sqrt2;//link7


			for (k = 0; k < 8; ++k)
			{
				if (max < ilc[index + k])
					max = ilc[index + k];
			}
		}
	}
	int length1[] = { 1, 3, 4, 6 };
	int lengths2[] = { 0, 2, 5, 7 };
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			int index = i*cols * 8 + j * 8;
			int k = 0;
			for (; k < 4; ++k)
			{
				ilc[index + length1[k]] = max - ilc[index + length1[k]];
			}
			for (k = 0; k < 4; ++k)
			{
				ilc[index + lengths2[k]] = (max - ilc[index + lengths2[k]])*sqrt2;
			}
		}
	}

}