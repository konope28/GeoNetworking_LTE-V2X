#include "gucpacket.h"

GUCPacket::GUCPacket(LPV* so_pv, SPV* de_pv, int current_time)
{
	guc_head = new GUCHeader(so_pv, de_pv, current_time);
}

GUCPacket::~GUCPacket(){}

BasicHeader* GUCPacket::GetBasicHeader()
{
	return  guc_head->GetBasicHeader();
}
CommonHeader* GUCPacket::GetCommonHeader()
{
	return guc_head->GetCommonHeader();
}
LPV* GUCPacket::GetSOPV()
{
	return guc_head->GetSOPV();
}
SPV* GUCPacket::GetDEPV()
{
	return guc_head->GetDEPV();
}
int GUCPacket::GetSN()
{
	return guc_head->GetSN();
}
void GUCPacket::UpdateDEPV(double x, double y)
{
	GetDEPV()->SetX(x);
	GetDEPV()->SetY(y);
}