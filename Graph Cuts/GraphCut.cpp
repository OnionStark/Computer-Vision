#include"GraphCut.h"
#include"general.h"
#include"GlobalClass.h"
#include"XjMat.h"
typedef unsigned int uint;
const char* NAME = "working";
//4领域
int Noffset[4];//偏移量 
int PRE[4] = { 2,3,0,1 };
MarkCut* t = nullptr;

void graphcut(Mat& img)
{
	//如果不是三通道的，就转化为三通道
	Mat target;
	if (img.channels() == 3)
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
	namedWindow(NAME, WINDOW_AUTOSIZE);
	setMouseCallback(NAME, mouse_function, &param);

	cvShowImage(NAME, &iplImage);        //这样显示才能动态更新
	while (waitKey() != 13);

	//进行计算
	graphcut.IamWorking();
	cout << "正在计算最小割..." << endl;
	minCut(&graphcut);

	cout << "计算结束，正在分割图片..." << endl;
	//显示结果
	showInImage(&graphcut, &iplImage, t);
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
	case 3://中间点击
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

//最小割算法
void minCut(GraphCut* graph)
{
	vint queue;//队列
	int N = graph->img->rows*graph->img->cols;

	MarkCut* temp = new MarkCut[N+2]{};
	queue.push_back(N);//N表示s点，N+1表示T点
	Edge* e=nullptr;

	//先进行一些初始化先
	for (int i = 0; i < N; ++i)
	{
		e = graph->getEdge(N, i);
		Edge* et = nullptr;
		et = graph->getEdge(i, N + 1);
		et->f = e->f = min(e->ca, et->ca);
	}

	while (!queue.empty() )
	{
		int point = queue.back();
		queue.pop_back();
		graph->result.push_back(point);
		if (point == N) //s
		{
			//每条输出边
			for (int i = 0; i < N; ++i)
			{
				e = graph->getEdge(point, i);
				if (!temp[i].isMark() && e->isPush() )
				{
					temp[i].p = N;
					temp[i].f = e->getRemain();
					queue.push_back(i);
				}
			}
			//没有输入边
		}
		else
		{
			//每条输出边
			for (int i = 0; i < 5; ++i)
			{
				int nextP = N + 1;
				if (i < 4)
				{
					nextP = point + Noffset[i];
					if (nextP < 0 || nextP >= N)
						continue;
				}
				e = graph->getEdge(point, nextP);

				
				if (e->isPush() && !temp[nextP].isMark() )//对于无效的边也有用
				{
					temp[nextP].p = point;
					temp[nextP].f = min(e->getRemain(), temp[point].f);
					queue.push_back(nextP);
				}
			}

			//每条输入边
			for (int i = 0; i < 5; ++i)
			{
				int preP = N;
				if (i < 4)
				{
					preP = point + Noffset[i];
					if (preP < 0 || preP >= N) continue;
				}
				e = graph->getEdge(preP,point);

				if (!temp[preP].isMark() && e->isRepush())
				{
					temp[preP].p = -point;
					temp[preP].f = min(temp[point].f, e->f);
					queue.push_back(preP);
				}
			}

		}


		//如果t有标记
		if (temp[N + 1].isMark())
		{
			//开始增广
			//cout << temp[N+1].f << endl;
			int index = N + 1;
			while (index!=N)
			{
				e = graph->getEdge(abs(temp[index].p), index);
				if (temp[index].p > 0)
				{
					e->f += temp[index].f;
				}
				else
					e->f -= temp[index].f;
				index = abs(temp[index].p);
			}
			MarkCut::clear(temp, N + 2);
			graph->result.clear();
			queue.clear();
			queue.push_back(N);
		}
		//重新来
	}
	t = temp;
}


void showInImage(GraphCut* g, IplImage* img, MarkCut* temp)
{
	//从已标记顶点到未标记顶点的边就是割
	cloneImage(img, getGlobal()->img);
	int x, y;
	int cols = img->width;
	int N = img->height*cols;
	for (vint::iterator iter = g->result.begin(); iter != g->result.end(); ++iter)
	{
		if (*iter >= N) continue;
		//输出边
		for (int i = 0; i < 4; ++i)
		{
			int nextp = *iter + Noffset[i];
			if (nextp>=0 && nextp <N && !temp[nextp].isMark())
			{
				x = *iter%cols, y = *iter / cols;
				int index = y*cols * 3 + x * 3;
				img->imageData[index] = 0;
				img->imageData[index + 1] = 0;
				img->imageData[index + 2] = 255;

				x = nextp%cols, y = nextp / cols;
				index = y*cols * 3 + x * 3;
				img->imageData[index] = 0;
				img->imageData[index + 1] = 0;
				img->imageData[index + 2] = 255;
			}
		}
	}
	delete[] temp;
	cvShowImage(NAME, img);
	waitKey();

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
		delete[] mark;
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

	float Err = -1.0f;//无效的权值，表示此路不通
	float keno = getGlobal()->keno;
	float lambda = getGlobal()->lambda;
	float max = 0.0f; //K
	const float TEMP = 0.5f / keno / keno;

	//先算各个邻域的
	for (int i = 0; i < N; ++i)
	{
		//统计直方图的变量
		if (mark[i] == 'o')
		{
			++account.o[ img->data[i] ];
			++account.totalo;
		}
		else if (mark[i] == 'b')
		{
			++account.b[img->data[i]];
			++account.totalb;
		}

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

	if (account.totalo == 0 || account.totalb == 0)
	{
		cerr << "user didn't mark the seed point!" << endl;
		exit(0);
	}


	//处理s 和 t
	for (int i = 0; i < N; ++i)
	{
		if (mark[i] == getGlobal()->nullMark)
		{
			uchar index = img->data[i];
			float pro = account.o[index] / (float)account.totalo;
			float prb = account.b[index] / (float)account.totalb;
			if (pro == 0)
				pro = 1.0f / account.totalo;
			if (prb == 0)
				prb = 1.0f / account.totalb;
			float temp = pro + prb;
			pro /= temp, prb /= temp;
			sourceE[i].ca = -lambda*log(prb);
			pointE[i * 5 + 4].ca = -lambda*log(pro);
		}
		else if (mark[i] == 'o')
		{
			sourceE[i].ca = max;
			pointE[i * 5 + 4].ca = 0.f;
		}
		else if (mark[i] == 'b')
		{
			sourceE[i].ca = 0.f;
			pointE[i * 5 + 4].ca = max;
		}
		else
		{
			cerr << "不可能会执行到这一步！" << endl;
			exit(0);
		}
	}


	//cout << "source" << endl;
	//for (int i = 0; i < N; ++i)
	//{
	//	cout << i%cols << "  " << i / cols << "  "<<mark[i]<<"  :";
	//	cout << "sorurce : "<<sourceE[i].ca << endl;
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		cout << pointE[i * 5 + j].ca << "  ";
	//	}
	//	cout << endl;
	//	cout << "tt   :  " << pointE[i * 5 + 4].ca << endl;
	//}

}

// u --> v
Edge* GraphCut::getEdge(int u, int v)
{
	int N = img->rows*img->cols;
	//s == N
	//t == N+1
	if (u < N && v<N)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (u + Noffset[i] == v)
			{
				return pointE+u * 5 + i;
			}
		}
		return nullptr;//没有找到
	}
	else if (u == N)
	{
		return sourceE + v;
	}
	else if (v == N + 1)
	{
		return pointE + u * 5 + 4;
	}
}