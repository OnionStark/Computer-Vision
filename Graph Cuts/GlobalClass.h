#ifndef _GLOBALCLASS
#define _GLOBALCLASS
#include"base.h"
struct GlobalClass
{
private:
	GlobalClass() :R(5.f), img(nullptr) {}
	GlobalClass(const GlobalClass&){}
public:


	static float keno;
	static float lambda;
	float R;//画笔的半径
	IplImage *img;//保存一份原始的画稿


	~GlobalClass();
	void setImage(const IplImage* p);

	friend GlobalClass* getGlobal();
};


inline GlobalClass* getGlobal()
{
	static GlobalClass g;
	return &g;
}

#endif