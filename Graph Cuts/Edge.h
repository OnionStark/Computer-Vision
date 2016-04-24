#ifndef _EDGE
#define _EDGE

struct Edge
{
	float ca, f;//分别表示一个边的容量和流量
	Edge(const float c = 0.0f, const float inf = 0.0f):
		ca(c), f(inf) {}
	bool isPush() const { return (ca > f); } //是否还能继续添加
	float getRemain() const { return ca - f; }
	bool isRepush() const { return f > 0.f; }
};

#endif
