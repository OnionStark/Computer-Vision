#include"XjMat.h"


//--------------xjMat----------------------

xjMat::xjMat(unsigned char* d, int r, int c, int inc) :
	data(d), rows(r), cols(c), channels(inc)
{

}


xjMat::xjMat(const Mat& M)
{
	if (!M.isContinuous())
	{
		cerr << "error, the matrix's data is not continuous!" << endl;
		exit(1);
	}
	data = M.data;
	rows = M.rows;
	cols = M.cols;
	channels = M.channels();
}
