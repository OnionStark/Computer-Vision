#include"GlobalClass.h"

void GlobalClass::reset()
{
	isFirst = false;
	if (imgptr != nullptr)
		cvReleaseImage(&imgptr);
	imgptr = nullptr;
}
GlobalClass::~GlobalClass()
{
	if (imgptr != nullptr)
		cvReleaseImage(&imgptr);
}
void GlobalClass::setImage(const IplImage* img)
{
	if (imgptr != nullptr)
		cvReleaseImage(&imgptr);
	imgptr = cvCloneImage(img);
}

void GlobalClass::updateImg(int x, int y, IntellSci* ins)
{
	if (px == x && py == y) return;
	int cols = imgptr->width;
	int free = y*cols + x;
	int seed = py*cols + px;
	int* p = ins->getP();
	int tempx, tempy;

	while (p[free] != seed)
	{
		tempx = free%cols, tempy = free / cols;
		imgptr->imageData[tempy*cols * 3 + tempx*3] = 0;
		imgptr->imageData[tempy*cols * 3 + tempx*3+1] = 0;
		imgptr->imageData[tempy*cols * 3 + tempx*3+2] = 255;
		free = p[free];
	}
	px = x, py = y;
}