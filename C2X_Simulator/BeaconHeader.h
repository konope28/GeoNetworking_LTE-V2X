#ifndef HEADERS_BEACONHEADER_H_
#define HEADERS_BEACONHEADER_H_
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"

class BeaconHeader
{
public:
	BeaconHeader(LPV* so_pv, int current_time);
	~BeaconHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	int GetSN();

private:
	BasicHeader* basic_head;
	CommonHeader* common_head;
	LPV* SO_PV;
	static int sequence_number;
	int SN;
};

#endif /*HEADERS_BEACONHEADER_H_*/
