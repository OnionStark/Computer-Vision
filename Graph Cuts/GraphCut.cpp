#include"GraphCut.h"
#include"general.h"
#include"GlobalClass.h"
#include"XjMat.h"
typedef unsigned int uint;
const char* NAME = "working";
//4����
int Noffset[4];//ƫ���� 
int PRE[4] = { 2,3,0,1 };
MarkCut* t = nullptr;

void graphcut(Mat& img)
{
	//���������ͨ���ģ���ת��Ϊ��ͨ��
	Mat target;
	if (img.channels() == 3)
	{
		converToGray(target, img);
	}
	//img ��Ϊ��ʾ�Ļ�������target �������ͺ���
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

	//��get���û���ǵ���Ϣ
	namedWindow(NAME, WINDOW_AUTOSIZE);
	setMouseCallback(NAME, mouse_function, &param);

	cvShowImage(NAME, &iplImage);        //������ʾ���ܶ�̬����
	while (waitKey() != 13);

	//���м���
	graphcut.IamWorking();
	cout << "���ڼ�����С��..." << endl;
	minCut(&graphcut);

	cout << "������������ڷָ�ͼƬ..." << endl;
	//��ʾ���
	showInImage(&graphcut, &iplImage, t);
}

void mouse_function(int mEvent, int x, int y, int flags, void* p)
{
	//���1 ��4�� ��ס1
	//�Ҽ�2�� 5 �� ��ס2
	//�м� 3�� 6�� ��ס 4
	//������ɫ��������ɫ
	FuncParam* param = reinterpret_cast<FuncParam*>(p);
	IplImage* img = param->img;
	GraphCut* graph = param->graphcut;
	int type=0;//ָʾ����߻����ұߵĻ���
	auto f = [&type, &graph](IplImage* img,int x,int y)->void
	{
		int cols = img->width;
		bool is = true;
		//�ӽ�ȥ
		uchar r, g, b;

		switch (type)
		{
		case 1://��ɫ
		{
			r = b = 0;
			g = 255;
			graph->getMark()[y*cols + x] = 'o';
			break;
		}
		case 2://��ɫ
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
	case CV_EVENT_LBUTTONDOWN://������
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
	case 2://�Ҽ����
	{
		type = 2;
		printRadiu(img, x, y, getGlobal()->R, f);
		break;
	}
	case 3://�м���
	{
		//����
		graph->clearMark();
		cloneImage(img, getGlobal()->img);
		break;
	}
	default :
		break;
	}
	cvShowImage(NAME, img);
}

//��С���㷨
void minCut(GraphCut* graph)
{
	vint queue;//����
	int N = graph->img->rows*graph->img->cols;

	MarkCut* temp = new MarkCut[N+2]{};
	queue.push_back(N);//N��ʾs�㣬N+1��ʾT��
	Edge* e=nullptr;

	//�Ƚ���һЩ��ʼ����
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
			//ÿ�������
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
			//û�������
		}
		else
		{
			//ÿ�������
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

				
				if (e->isPush() && !temp[nextP].isMark() )//������Ч�ı�Ҳ����
				{
					temp[nextP].p = point;
					temp[nextP].f = min(e->getRemain(), temp[point].f);
					queue.push_back(nextP);
				}
			}

			//ÿ�������
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


		//���t�б��
		if (temp[N + 1].isMark())
		{
			//��ʼ����
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
		//������
	}
	t = temp;
}


void showInImage(GraphCut* g, IplImage* img, MarkCut* temp)
{
	//���ѱ�Ƕ��㵽δ��Ƕ���ı߾��Ǹ�
	cloneImage(img, getGlobal()->img);
	int x, y;
	int cols = img->width;
	int N = img->height*cols;
	for (vint::iterator iter = g->result.begin(); iter != g->result.end(); ++iter)
	{
		if (*iter >= N) continue;
		//�����
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
	pointE = new Edge[N * 5];//ÿ��������5�������
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
	} account; //ͳ��ֱ��ͼ�ı���
	account.totalo = account.totalb = 0;
	int cols = img->cols;
	int N = img->rows*img->cols;

	float Err = -1.0f;//��Ч��Ȩֵ����ʾ��·��ͨ
	float keno = getGlobal()->keno;
	float lambda = getGlobal()->lambda;
	float max = 0.0f; //K
	const float TEMP = 0.5f / keno / keno;

	//������������
	for (int i = 0; i < N; ++i)
	{
		//ͳ��ֱ��ͼ�ı���
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

		//ÿ�����ش���link1 �� link2
		int q = i + Noffset[1];//link1
		if (q >= 0)
		{
			pointE[i * 5 + 1].ca = exp(-(img->data[i] - img->data[q])
				*(img->data[i] - img->data[q])*TEMP);
			//���¶�Ӧ����link3��cost,
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
			//���¶�Ӧ�����link1��cost
			pointE[q * 5].ca = pointE[i * 5 + 2].ca;
			if (max < pointE[i * 5 + 2].ca)
				max = pointE[i * 5 + 2].ca;
		}
		else//�ڱ߽磬��Ч�ı�
		{
			pointE[i * 5 + 2].ca = Err;
		}
		
	}

	//����0 �� 3 ��link
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


	//����s �� t
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
			cerr << "�����ܻ�ִ�е���һ����" << endl;
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
		return nullptr;//û���ҵ�
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