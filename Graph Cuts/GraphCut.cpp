#include"GraphCut.h"
#include"general.h"
#include"GlobalClass.h"
#include"XjMat.h"
typedef unsigned int uint;
const char* NAME = "working";
//4����
int Noffset[4];//ƫ���� 

void graphcut(Mat& img)
{
	//���������ͨ���ģ���ת��Ϊ��ͨ��
	Mat target;
	if (img.channels() != 3)
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
	cout << "����ͼƬ�ϱ�ǳ�����ͱ���(��ס����ƶ�����������壬�Ҽ��������м�������enter����" << endl;
	namedWindow(NAME, WINDOW_AUTOSIZE);
	setMouseCallback(NAME, mouse_function, &param);

	cvShowImage(NAME, &iplImage);        //������ʾ���ܶ�̬����
	while(waitKey()!='q') ;

	//���м���
	graphcut.IamWorking();
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
	case 5://�м���
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
	for (int i = 0; i < N; ++i)
	{
		//ͳ��ֱ��ͼ�ı���
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

	float Err = -1.0f;//��Ч��Ȩֵ����ʾ��·��ͨ
	float keno = getGlobal()->keno;
	float lambda = getGlobal()->lambda;
	float max = 0.0f; //K
	const float TEMP = 0.5f / keno / keno;

	//������������
	for (int i = 0; i < N; ++i)
	{


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
		

		//����S
		if (mark[i] == 'p')
		{

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


	//����s �� t
}