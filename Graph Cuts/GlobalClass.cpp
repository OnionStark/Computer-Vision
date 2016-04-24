#include"GlobalClass.h"
float GlobalClass::keno = 0.46f;
float GlobalClass::lambda = 1.f;

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