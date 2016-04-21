#include"XjMat.h"
//--------------xjMat----------------------

xjMat::xjMat(unsigned char* d, int r, int c, int inc) :
	data(d), rows(r), cols(c), channels(inc)
{
	

}


xjMat::xjMat(const Mat& M)
{
	M.ptr<unsigned char>();
	if (!M.isContinuous())
	{
		cerr << "error, the matrix's data is not continous!" << endl;
		exit(1);
	}
	data = M.data;
	rows = M.rows;
	cols = M.cols;
	channels = M.channels();
}
