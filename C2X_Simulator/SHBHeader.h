#ifndef HEADERS_SHBHEADER_H_
#define HEADERS_SHBHEADER_H_
#include <stdlib.h>
#include <time.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
using namespace std;


class SHBHeader
{
public:
	SHBHeader(LPV* so_pv, int current_time);
	~SHBHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	int GetSN();

private:
	BasicHeader* basic_head;
	CommonHeader* common_head;
	static int sequence_number;
	int SN;
	LPV* SO_PV;
};

#endif /*HEADERS_SHBHEADER_H_*/
