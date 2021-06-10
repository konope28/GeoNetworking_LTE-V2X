#ifndef HEADERS_LSREQHEADER_H_
#define HEADERS_LSREQHEADER_H_
#include <stdlib.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
using namespace std;


class LSReqHeader
{
public:
	LSReqHeader(LPV* so_pv, string gn_addr, int current_time);
	~LSReqHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	int GetSN();
	string GetReq_GN_ADDR();
	void SetSN(int sn);
	
private:
	static int sequence_number;
	BasicHeader* basic_head;
	CommonHeader* common_head;
	int SN;
	LPV* SO_PV;
	string req_GN_ADDR;
};

#endif /*HEADERS_LSREQHEADER_H_*/