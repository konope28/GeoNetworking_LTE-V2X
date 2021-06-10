#include "spv.h"



SPV::SPV(string gn_addr, double x, double y, int t)
{
	GN_ADDR = gn_addr;
	coord_X = x;
	coord_Y = y;
	timestamp = t;
}
SPV::~SPV(){}

string SPV::GetGN_ADDR()
{
	return GN_ADDR;
}
double SPV::GetX()
{
	return coord_X;
}
double SPV::GetY()
{
	return coord_Y;
}
int SPV::GetTimestamp()
{
	return timestamp;
}
void SPV::SetX(double x)
{
	coord_X = x;
}
void SPV::SetY(double y)
{
	coord_Y = y;
}
void SPV::SetTimestamp(int t)
{
	timestamp = t;
}