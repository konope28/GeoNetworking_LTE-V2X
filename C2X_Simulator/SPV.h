#ifndef DATASTRUCTURES_SPV_H_
#define DATASTRUCTURES_SPV_H_
#include <string>
using namespace std;


class SPV
{
public:
	SPV(string gn_addr, double x, double y, int t);
	~SPV();
	string GetGN_ADDR();
	double GetX();
	double GetY();
	int GetTimestamp();
	void SetX(double x);
	void SetY(double y);
	void SetTimestamp(int t);

private:
	string GN_ADDR;
	double coord_X;
	double coord_Y;
	int timestamp;
};

#endif /*DATASTRUCTURES_SPV_H_*/
