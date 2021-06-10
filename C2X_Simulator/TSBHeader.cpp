#include "tsbheader.h"
int TSBHeader::sequence_number = 0;

TSBHeader::TSBHeader(LPV* so_pv, int hops, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, hops);	//LT = 600 (60s is default), RHL = 10
	common_head = new CommonHeader(5, 1, 0, hops); //HT=5, HST=1, PL=0, MHL=10
	SO_PV = so_pv;
	SN = 40000 + ++sequence_number;
}
TSBHeader::~TSBHeader(){}

BasicHeader* TSBHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* TSBHeader::GetCommonHeader()
{
	return common_head;
}
LPV* TSBHeader::GetSOPV()
{
	return SO_PV;
}
int TSBHeader::GetSN()
{
	return SN;
}
void TSBHeader::SetSN(int sn)
{
	SN = sn;
}