#include "tsbpacket.h"

TSBPacket::TSBPacket(LPV* so_pv, int hops, int current_time)
{
	tsb_head = new TSBHeader(so_pv, hops, current_time);
}

TSBPacket::~TSBPacket(){}

BasicHeader* TSBPacket::GetBasicHeader()
{
	return  tsb_head->GetBasicHeader();
}
CommonHeader* TSBPacket::GetCommonHeader()
{
	return tsb_head->GetCommonHeader();
}
LPV* TSBPacket::GetSOPV()
{
	return tsb_head->GetSOPV();
}
int TSBPacket::GetSN()
{
	return tsb_head->GetSN();
}
void TSBPacket::SetSN(int sn)
{
	tsb_head->SetSN(sn);
}