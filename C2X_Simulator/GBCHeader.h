#ifndef HEADERS_GACHEADER_H_
#define HEADERS_GACHEADER_H_
#include <stdlib.h>
#include "basicheader.h"
#include "commonheader.h"
#include "lpv.h"
using namespace std;

class GBCHeader
{
public:
	GBCHeader(LPV* so_pv, int dest_x, int dest_y, int radious, int current_time);
	~GBCHeader();

	BasicHeader* GetBasicHeader();
	CommonHeader* GetCommonHeader();
	LPV* GetSOPV();
	int GetSN();
	int GetDestX();
	int GetDestY();
	int GetRadius();
	void SetSN(int sn);

private:
	static int sequence_number;
	BasicHeader* basic_head;
	CommonHeader* common_head;
	int SN;
	LPV* SO_PV;
	double dest_X;
	double dest_Y;
	double radius;
};

#endif /*HEADERS_GBCHEADER_H_*/
