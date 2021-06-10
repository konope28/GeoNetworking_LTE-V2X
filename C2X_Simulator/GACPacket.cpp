#include "gacpacket.h"

GACPacket::GACPacket(LPV* so_pv, int dest_x, int dest_y, int radious, int current_time)
{
	gac_head = new GACHeader(so_pv, dest_x, dest_y, radious, current_time);
}

GACPacket::~GACPacket(){}

BasicHeader* GACPacket::GetBasicHeader()
{
	return  gac_head->GetBasicHeader();
}
CommonHeader* GACPacket::GetCommonHeader()
{
	return gac_head->GetCommonHeader();
}
LPV* GACPacket::GetSOPV()
{
	return gac_head->GetSOPV();
}
int GACPacket::GetSN()
{
	return gac_head->GetSN();
}
int GACPacket::GetDestX()
{
	return gac_head->GetDestX();
}
int GACPacket::GetDestY()
{
	return gac_head->GetDestY();
}
int GACPacket::GetRadius()
{
	return gac_head->GetRadius();
}
string GACPacket::GetSenderGN_ADDR()
{
	return sender_GN_ADDR;
}
void GACPacket::SetSenderGN_ADDR(string gn)
{
	sender_GN_ADDR = gn;
}
void GACPacket::SetSN(int sn)
{
	gac_head->SetSN(sn);
}