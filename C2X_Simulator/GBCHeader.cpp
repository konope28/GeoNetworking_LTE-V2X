#include "gbcheader.h"

int GBCHeader::sequence_number = 0;

GBCHeader::GBCHeader(LPV* so_pv, int x, int y, int r, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 10);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(4, 0, 0, 10); //HT=4, HST=0, PL=0, MHL=10
	SO_PV = so_pv;
	dest_X = x;
	dest_Y = y;
	radius = r;
	SN = 60000 + ++sequence_number;
}
GBCHeader::~GBCHeader(){}

BasicHeader* GBCHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* GBCHeader::GetCommonHeader()
{
	return common_head;
}
LPV* GBCHeader::GetSOPV()
{
	return SO_PV;
}
int GBCHeader::GetSN()
{
	return SN;
}
int GBCHeader::GetDestX()
{
	return dest_X;
}
int GBCHeader::GetDestY()
{
	return dest_Y;
}
int GBCHeader::GetRadius()
{
	return radius;
}
void GBCHeader::SetSN(int sn)
{
	SN = sn;
}