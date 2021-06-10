#include "lpv.h"


LPV::LPV(string gn_addr, double x, double y, double v, double h, int t)
{
	GN_ADDR = gn_addr;
	coord_X = x;
	coord_Y = y;
	velocity = v;
	heading = h;
	timestamp = t;
}
LPV::~LPV(){}

string LPV::GetGN_ADDR()
{
	return GN_ADDR;
}
double LPV::GetX()
{
	return coord_X;
}
double LPV::GetY()
{
	return coord_Y;
}
double LPV::GetV()
{
	return velocity;
}
double LPV::GetH()
{
	return heading;
}
int LPV::GetTimestamp()
{
	return timestamp;
}
void LPV::SetX(double x)
{
	coord_X = x;
}
void LPV::SetY(double y)
{
	coord_Y = y;
}
void LPV::SetV(double v)
{
	velocity = v;
}
void LPV::SetH(double h)
{
	heading = h;
}
void LPV::SetTimestamp(int t)
{
	timestamp = t;
}