#include "shbheader.h"

int SHBHeader::sequence_number = 0;

SHBHeader::SHBHeader(LPV* so_pv, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 1);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(5, 0, 0, 1); //HT=5, HST=0, PL=0, MHL=10
	SO_PV = so_pv;
	SN = 80000 + ++sequence_number;
}
SHBHeader::~SHBHeader(){}

BasicHeader* SHBHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* SHBHeader::GetCommonHeader()
{
	return common_head;
}
LPV* SHBHeader::GetSOPV()
{
	return SO_PV;
}
int SHBHeader::GetSN()
{
	return SN;
}
