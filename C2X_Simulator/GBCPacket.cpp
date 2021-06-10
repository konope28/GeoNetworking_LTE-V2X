#include "gbcpacket.h"

GBCPacket::GBCPacket(LPV* so_pv, int dest_x, int dest_y, int radious, int current_time)
{
	gbc_head = new GBCHeader(so_pv, dest_x, dest_y, radious, current_time);
}

GBCPacket::~GBCPacket(){}

BasicHeader* GBCPacket::GetBasicHeader()
{
	return  gbc_head->GetBasicHeader();
}
CommonHeader* GBCPacket::GetCommonHeader()
{
	return gbc_head->GetCommonHeader();
}
LPV* GBCPacket::GetSOPV()
{
	return gbc_head->GetSOPV();
}
int GBCPacket::GetSN()
{
	return gbc_head->GetSN();
}
int GBCPacket::GetDestX()
{
	return gbc_head->GetDestX();
}
int GBCPacket::GetDestY()
{
	return gbc_head->GetDestY();
}
int GBCPacket::GetRadius()
{
	return gbc_head->GetRadius();
}
string GBCPacket::GetSenderGN_ADDR()
{
	return sender_GN_ADDR;
}
void GBCPacket::SetSenderGN_ADDR(string gn)
{
	sender_GN_ADDR = gn;

}
void GBCPacket::SetSN(int sn)
{
	gbc_head->SetSN(sn);
}
