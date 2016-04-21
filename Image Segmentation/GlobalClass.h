#ifndef _GLOABLCLASS
#define _GLOBALCLASS
#include"base.h"
#include"IntelligentScissors.h"
//全局类，单例模式，用于控制交互和显示剪刀
class GlobalClass
{

	GlobalClass() :imgptr(nullptr) {}
	GlobalClass(const GlobalClass&) {} //单例模式不允许从外面生成
public:


	IplImage* imgptr;   //用于显示的图像指针
	bool isFirst;   //用于第一次生成最小路径图
	int px, py;

	~GlobalClass();
	void reset();   //每次重新设置初值
	void setImage(const IplImage*);
	void updateImg(int, int, IntellSci*);  //更新图像信息的函数
	friend GlobalClass& getGlobal();
};
inline GlobalClass& getGlobal()
{
	static GlobalClass g;
	return g;
}


#endif _GLOBALCLASS