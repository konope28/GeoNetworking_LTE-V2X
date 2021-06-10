#include "shbpacket.h"

SHBPacket::SHBPacket(LPV* so_pv, int current_time)
{
	shb_head = new SHBHeader(so_pv, current_time);
}

SHBPacket::~SHBPacket(){}

BasicHeader* SHBPacket::GetBasicHeader()
{
	return  shb_head->GetBasicHeader();
}
CommonHeader* SHBPacket::GetCommonHeader()
{
	return shb_head->GetCommonHeader();
}
LPV* SHBPacket::GetSOPV()
{
	return shb_head->GetSOPV();
}
int SHBPacket::GetSN()
{
	return shb_head->GetSN();
}