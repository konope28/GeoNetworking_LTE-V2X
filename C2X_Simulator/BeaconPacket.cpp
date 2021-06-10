#include "beaconpacket.h"

BeaconPacket::BeaconPacket(LPV* lpv, int current_time): Packet()
{
	beacon_head = new BeaconHeader(lpv, current_time);
}
BeaconPacket::~BeaconPacket(){}

BasicHeader* BeaconPacket::GetBasicHeader()
{
	return beacon_head->GetBasicHeader();
}
CommonHeader* BeaconPacket::GetCommonHeader() 
{
	return beacon_head->GetCommonHeader();
}
LPV* BeaconPacket::GetSOPV()
{
	return beacon_head->GetSOPV();
}
int BeaconPacket::GetSN()
{
	return beacon_head->GetSN();
}