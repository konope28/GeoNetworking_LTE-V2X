#ifndef DATASTRUCTURES_LPV_H_
#define DATASTRUCTURES_LPV_H_
#include <string>
using namespace std;

class LPV
{
public:
	LPV(string gn_addr, double x, double y, double v, double h, int t);
	~LPV();
	string GetGN_ADDR();
	double GetX();
	double GetY();
	double GetV();
	double GetH();
	int GetTimestamp();
	void SetX(double x);
	void SetY(double y);
	void SetV(double v);
	void SetH(double h);
	void SetTimestamp(int t);

private:
	string GN_ADDR;
	double coord_X;
	double coord_Y;
	double velocity;
	double heading;
	int timestamp;
};

#endif /*DATASTRUCTURES_LPV_H_*/
