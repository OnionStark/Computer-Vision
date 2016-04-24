#ifndef _EDGE
#define _EDGE

struct Edge
{
	float ca, f;//分别表示一个边的容量和流量
	Edge(const float c = 0.0f, const float inf = 0.0f):
		ca(c), f(inf) {}
};

#endif
