#ifndef _EDGE
#define _EDGE

struct Edge
{
	float ca, f;//�ֱ��ʾһ���ߵ�����������
	Edge(const float c = 0.0f, const float inf = 0.0f):
		ca(c), f(inf) {}
};

#endif
