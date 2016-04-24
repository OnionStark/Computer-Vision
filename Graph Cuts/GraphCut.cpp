#include"GraphCut.h"
#include"general.h"
#include"GlobalClass.h"
#include"XjMat.h"
typedef unsigned int uint;
const char* NAME = "working";
//4领域
int Noffset[4];//偏移量 

void graphcut(Mat& img)
{
	//如果不是三通道的，就转化为三通道
	Mat target;
	if (img.channels() != 3)
	{
		converToGray(target, img);
	}
	//img 作为显示的基础，在target 上面计算就好了
	else
	{
		target = img.clone();
	}

	IplImage iplImage = img;
	getGlobal()->setImage(&iplImage);

	GraphCut graphcut(&target);
	graphcut.init();
	Noffset[0] = 1;
	Noffset[1] = -img.cols;
	Noffset[2] = -1;
	Noffset[3] = img.cols;


	FuncParam param = { &graphcut, &iplImage };

	//先get到用户标记的信息
	cout << "请在图片上标记出物体和背景(按住鼠标移动），左键物体，右键背景，中键重来，enter继续" << endl;
	namedWindow(NAME, WINDOW_AUTOSIZE);
	setMouseCallback(NAME, mouse_function, &param);

	cvShowImage(NAME, &iplImage);        //这样显示才能动态更新
	while(waitKey()!='q') ;

	//进行计算
	graphcut.IamWorking();
}

void mouse_function(int mEvent, int x, int y, int flags, void* p)
{
	//左键1 下4上 按住1
	//右键2下 5 上 按住2
	//中间 3下 6上 按住 4
	//物体绿色，背景蓝色
	FuncParam* param = reinterpret_cast<FuncParam*>(p);
	IplImage* img = param->img;
	GraphCut* graph = param->graphcut;
	int type=0;//指示是左边还是右边的画笔
	auto f = [&type, &graph](IplImage* img,int x,int y)->void
	{
		int cols = img->width;
		bool is = true;
		//加进去
		uchar r, g, b;

		switch (type)
		{
		case 1://绿色
		{
			r = b = 0;
			g = 255;
			graph->getMark()[y*cols + x] = 'o';
			break;
		}
		case 2://蓝色
		{
			r = g = 0;
			b = 255;
			graph->getMark()[y*cols + x] = 'b';
			break;
		}
		default:
			is = false;
			break;
		}
		if (is)
		{
			img->imageData[y*cols * 3 + x * 3] = b;
			img->imageData[y*cols * 3 + x * 3 + 1] = g;
			img->imageData[y*cols * 3 + x * 3 + 2] = r;
		}
	};
	switch (mEvent)
	{
	case CV_EVENT_LBUTTONDOWN://左键点击
	{
		type = 1;
		printRadiu(img, x, y, getGlobal()->R, f);
		break;
	}
	case CV_EVENT_MOUSEMOVE:
	{
		if(flags == 0)
			return;
		type = flags;
		printRadiu(img, x, y, getGlobal()->R, f);
		break;
	}
	case 2://右键点击
	{
		type = 2;
		printRadiu(img, x, y, getGlobal()->R, f);
		break;
	}
	case 5://中间点击
	{
		//归零
		graph->clearMark();
		cloneImage(img, getGlobal()->img);
		break;
	}
	default :
		break;
	}
	cvShowImage(NAME, img);
}


//-----------GraphCut------------
void GraphCut::init()
{
	if (img == nullptr)
	{
		cerr << "In GraphCut::init, image is nullptr!" << endl;
		exit(0);
	}
	int N = img->rows*img->cols;
	if (pointE != nullptr)
		delete[] pointE;
	if (sourceE != nullptr)
		delete[] sourceE;
	pointE = new Edge[N * 5];//每个顶点有5条输出边
	sourceE = new Edge[N];

	if (mark != nullptr)
		mark = new char[N];

	clearMark();

}

GraphCut::~GraphCut()
{
	if (pointE != nullptr)
	{
		delete[] pointE;
	}
	if (sourceE != nullptr)
		delete[] sourceE;
	if (mark != nullptr)
		delete[] mark;
}

void GraphCut::IamWorking()
{
	computeCost();
}

void GraphCut::computeCost()
{
	struct
	{
		uint o[256]{ 0 };
		uint b[256]{ 0 };
		uint totalo;
		uint totalb;
	} account; //统计直方图的变量
	account.totalo = account.totalb = 0;
	int cols = img->cols;
	int N = img->rows*img->cols;
	for (int i = 0; i < N; ++i)
	{
		//统计直方图的变量
		if (mark[i] == 'o')
		{
			++account.o[i];
			++account.totalo;
		}
		else if (mark[i] == 'b')
		{
			++account.b[i];
			++account.totalb;
		}
	}

	float Err = -1.0f;//无效的权值，表示此路不通
	float keno = getGlobal()->keno;
	float lambda = getGlobal()->lambda;
	float max = 0.0f; //K
	const float TEMP = 0.5f / keno / keno;

	//先算各个邻域的
	for (int i = 0; i < N; ++i)
	{


		//每个像素处理link1 和 link2
		int q = i + Noffset[1];//link1
		if (q >= 0)
		{
			pointE[i * 5 + 1].ca = exp(-(img->data[i] - img->data[q])
				*(img->data[i] - img->data[q])*TEMP);
			//更新对应邻域link3的cost,
			pointE[q * 5 + 3].ca = pointE[i * 5 + 1].ca;
			if (max < pointE[i * 5 + 1].ca)
				max = pointE[i * 5 + 1].ca;
		}
		else
		{
			pointE[i * 5 + 1].ca = Err;
		}

		q = i + Noffset[2];//link2
		if (i%cols != 0)
		{
			pointE[i*5+2].ca = exp(-(img->data[i] - img->data[q])
				*(img->data[i] - img->data[q])*TEMP);
			//更新对应领域的link1的cost
			pointE[q * 5].ca = pointE[i * 5 + 2].ca;
			if (max < pointE[i * 5 + 2].ca)
				max = pointE[i * 5 + 2].ca;
		}
		else//在边界，无效的边
		{
			pointE[i * 5 + 2].ca = Err;
		}
		

		//处理S
		if (mark[i] == 'p')
		{

		}
	}

	//处理0 和 3 的link
	for (int i = cols - 1; i < N; i += cols)
	{
		pointE[i * 5].ca = Err;
	}
	for (int i = N - cols; i < N; ++i)
	{
		pointE[i*5+3].ca = Err;
	}


	//处理s 和 t
}