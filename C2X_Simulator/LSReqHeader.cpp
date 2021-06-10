#include "lsreqheader.h"

int LSReqHeader::sequence_number = 0;

LSReqHeader::LSReqHeader(LPV* so_pv, string gn_addr, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 10);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(6, 0, 0, 10); //HT=6, HST=0, PL=0, MHL=10
	SO_PV = so_pv;
	req_GN_ADDR = gn_addr;
	SN = 20000 + ++sequence_number;
}
LSReqHeader::~LSReqHeader() {}

BasicHeader* LSReqHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* LSReqHeader::GetCommonHeader()
{
	return common_head;
}
LPV* LSReqHeader::GetSOPV()
{
	return SO_PV;
}
int LSReqHeader::GetSN()
{
	return SN;
}
string LSReqHeader::GetReq_GN_ADDR()
{
	return req_GN_ADDR;
}
void LSReqHeader::SetSN(int sn)
{
	SN = sn;
}