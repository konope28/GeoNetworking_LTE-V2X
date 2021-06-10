#ifndef HEADERS_TSBHEADER_H_
#define HEADERS_TSBHEADER_H_
#include <stdlib.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
using namespace std;


class TSBHeader
{
public:
	TSBHeader(LPV* so_pv, int hops, int current_time);
	~TSBHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	int GetSN();
	void SetSN(int sn);

private:
	static int sequence_number;
	BasicHeader* basic_head;
	CommonHeader* common_head;
	int SN;
	LPV* SO_PV;
};

#endif /*HEADERS_TSBHEADER_H_*/
