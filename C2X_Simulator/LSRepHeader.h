#ifndef HEADERS_LSREPHEADER_H_
#define HEADERS_LSREPHEADER_H_
#include <stdlib.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
#include "spv.h"
using namespace std;

class LSRepHeader
{
public:
	LSRepHeader(LPV* so_pv, SPV* de_pv, int current_time);
	~LSRepHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	SPV* GetDEPV();
	int GetSN();

private:
	static int sequence_number;
	BasicHeader* basic_head;
	CommonHeader* common_head;
	int SN;
	LPV* SO_PV;
	SPV* DE_PV;
};

#endif /*HEADERS_LSREPHEADER_H_*/
