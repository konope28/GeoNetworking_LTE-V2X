#include "lsreqpacket.h"

LSReqPacket::LSReqPacket(LPV* so_pv, string gn_addr, int current_time)
{
	ls_req_head = new LSReqHeader(so_pv, gn_addr, current_time);
}

LSReqPacket::~LSReqPacket(){}

BasicHeader* LSReqPacket::GetBasicHeader()
{
	return  ls_req_head->GetBasicHeader();
}
CommonHeader* LSReqPacket::GetCommonHeader()
{
	return ls_req_head->GetCommonHeader();
}
LPV* LSReqPacket::GetSOPV()
{
	return ls_req_head->GetSOPV();
}
int LSReqPacket::GetSN()
{
	return ls_req_head->GetSN();
}
string LSReqPacket::GetReq_GN_ADDR()
{
	return ls_req_head->GetReq_GN_ADDR();
}
void LSReqPacket::SetSN(int sn)
{
	ls_req_head->SetSN(sn);
}