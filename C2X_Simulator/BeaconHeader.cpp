#include "beaconheader.h"

int BeaconHeader::sequence_number = 0;

BeaconHeader::BeaconHeader(LPV* so_pv, int current_time)
{
	basic_head = new BasicHeader(current_time + 600, 0);	//LT = 600 (60s is default), RHL = 0
	common_head = new CommonHeader(1, 0, 0, 0); //HT=1, HST=0, PL=0, MHL=0
	SO_PV = so_pv;
	SN = 90000 + ++sequence_number;
}
BeaconHeader::~BeaconHeader(){}

BasicHeader* BeaconHeader::GetBasicHeader()
{
	return basic_head;
}
CommonHeader* BeaconHeader::GetCommonHeader()
{
	return common_head;
}
LPV* BeaconHeader::GetSOPV()
{
	return SO_PV;
}
int BeaconHeader::GetSN()
{
	return SN;
}
