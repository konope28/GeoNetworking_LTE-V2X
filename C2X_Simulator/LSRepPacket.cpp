#include "lsreppacket.h"

LSRepPacket::LSRepPacket(LPV* so_pv, SPV* de_pv, int current_time)
{
	ls_rep_head = new LSRepHeader(so_pv, de_pv, current_time);
}

LSRepPacket::~LSRepPacket(){}

BasicHeader* LSRepPacket::GetBasicHeader()
{
	return  ls_rep_head->GetBasicHeader();
}
CommonHeader* LSRepPacket::GetCommonHeader()
{
	return ls_rep_head->GetCommonHeader();
}
LPV* LSRepPacket::GetSOPV()
{
	return ls_rep_head->GetSOPV();
}
SPV* LSRepPacket::GetDEPV()
{
	return ls_rep_head->GetDEPV();
}
int LSRepPacket::GetSN()
{
	return ls_rep_head->GetSN();
}
void LSRepPacket::UpdateDEPV(double x, double y)
{
	GetDEPV()->SetX(x);
	GetDEPV()->SetY(y);
}