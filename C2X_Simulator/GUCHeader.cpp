#include "gucheader.h"
int GUCHeader::sequence_number = 0;

GUCHeader::GUCHeader(LPV* so_pv, SPV* de_pv, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 10);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(2, 0, 0, 10); //HT=2, HST=0, PL=0, MHL=10
	SO_PV = so_pv;
	DE_PV = de_pv;
	SN = 10000 + ++sequence_number;
}
GUCHeader::~GUCHeader(){}

BasicHeader* GUCHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* GUCHeader::GetCommonHeader()
{
	return common_head;
}
LPV* GUCHeader::GetSOPV()
{
	return SO_PV;
}
SPV* GUCHeader::GetDEPV()
{
	return DE_PV;
}
int GUCHeader::GetSN()
{
	return SN;
}
