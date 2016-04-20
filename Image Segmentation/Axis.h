#ifndef _AXIS
#define _AXIS
//1x2 œÚ¡ø
template<typename T>
struct Axis
{
	T x, y;
	T& operator [] (int i)
	{
		if (i == 0)	return x;
		else  return y;
	}
	Axis() {}
	Axis(T inx, T iny) { x = inx, y = iny; }
	float getM() const { return sqrt(x*x + y*y); }
	friend Axis operator - (const Axis a, const Axis b)
	{
		return Axis(a.x - b.x, a.y - b.y);
	}
};

template<typename T, typename E>
float dot(const Axis<T>& a, const Axis<E>& b)
{
	return a.x*b.x + a.y* b.y;
}

#endif