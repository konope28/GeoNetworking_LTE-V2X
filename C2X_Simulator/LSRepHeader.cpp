#include "lsrepheader.h"
int LSRepHeader::sequence_number = 0;

LSRepHeader::LSRepHeader(LPV* so_pv, SPV* de_pv, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 10);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(6, 1, 0, 10); //HT=6, HST=1, PL=0, MHL=10
	SO_PV = so_pv;
	DE_PV = de_pv;
	SN = 30000 + ++sequence_number;
}
LSRepHeader::~LSRepHeader(){}

BasicHeader* LSRepHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* LSRepHeader::GetCommonHeader()
{
	return common_head;
}
LPV* LSRepHeader::GetSOPV()
{
	return SO_PV;
}
SPV* LSRepHeader::GetDEPV()
{
	return DE_PV;
}
int LSRepHeader::GetSN()
{
	return SN;
}