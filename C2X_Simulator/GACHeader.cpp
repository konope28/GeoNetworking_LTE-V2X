#include "gacheader.h"

int GACHeader::sequence_number = 0;

GACHeader::GACHeader(LPV* so_pv, int x, int y, int r, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 10);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(3, 0, 0, 10); //HT=3, HST=0, PL=0, MHL=10
	SO_PV = so_pv;
	dest_X = x;
	dest_Y = y;
	radius = r;
	SN = 50000 + ++sequence_number;
}
GACHeader::~GACHeader(){}

BasicHeader* GACHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* GACHeader::GetCommonHeader()
{
	return common_head;
}
LPV* GACHeader::GetSOPV()
{
	return SO_PV;
}
int GACHeader::GetSN()
{
	return SN;
}
int GACHeader::GetDestX()
{
	return dest_X;
}
int GACHeader::GetDestY()
{
	return dest_Y;
}
int GACHeader::GetRadius()
{
	return radius;
}
void GACHeader::SetSN(int sn)
{
	SN = sn;
}