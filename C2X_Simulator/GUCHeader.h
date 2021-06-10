#ifndef HEADERS_GUCHEADER_H_
#define HEADERS_GUCHEADER_H_
#include <stdlib.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
#include "spv.h"
using namespace std;

class GUCHeader
{
public:
	GUCHeader(LPV* so_pv, SPV* de_pv, int current_time);
	~GUCHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	SPV* GetDEPV();
	int GetSN();
	void SetSN();

private:
	static int sequence_number;
	BasicHeader* basic_head;
	CommonHeader* common_head;
	int SN;
	LPV* SO_PV;
	SPV* DE_PV;
};

#endif /*HEADERS_GUCHEADER_H_*/
