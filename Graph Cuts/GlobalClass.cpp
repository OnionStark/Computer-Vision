#include"GlobalClass.h"
float GlobalClass::keno = 1.f;
float GlobalClass::lambda = 1.f;
char GlobalClass::nullMark = 'p';

GlobalClass::~GlobalClass()
{
	if (img != nullptr)
	{
		cvReleaseImage(&img);
	}

}

void GlobalClass::setImage(const IplImage* p)
{
	if (img != nullptr)
	{
		cvReleaseImage(&img);
		img = nullptr;
	}
	img = cvCloneImage(p);
}
void GlobalClass::reset()
{
	cvReleaseImage(&img);
	img = nullptr;
}