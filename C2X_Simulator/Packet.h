#ifndef PACKETS_PACKET_H_
#define PACKETS_PACKET_H_
#include "basicheader.h"
#include "commonheader.h"
#include "beaconheader.h"
#include "lsreqheader.h"
#include "gucheader.h"

class Packet
{
public:
	Packet();
	~Packet();
	int GetReceptionTime();
	int GetTransmissionTime();
	void ScheduleTransmission(int time);
	void SetTransmissionTime(int time);
	double GetSenderX();
	double GetSenderY();
	void SetSenderX(double x);
	void SetSenderY(double y);
	int GetSubchannelIndex();
	int GetSubframeIndex();
	void SetSubchannelIndex(int c);
	void SetSubframeIndex(int f);

	virtual BasicHeader* GetBasicHeader() = 0;
	virtual CommonHeader* GetCommonHeader() = 0;
	virtual LPV* GetSOPV() = 0;
	virtual SPV* GetDEPV() = 0;
	virtual int GetSN() = 0;
	virtual string GetReq_GN_ADDR() = 0;
	virtual void UpdateDEPV(double x, double y) = 0;
	virtual void SetSN(int sn) = 0;
	virtual int GetDestX() = 0;
	virtual int GetDestY() = 0;
	virtual int GetRadius() = 0;
	virtual void SetSenderGN_ADDR(string g) = 0;
	virtual string GetSenderGN_ADDR() = 0;

private:
	int transmission_time;
	int reception_time;
	double sender_X;
	double sender_Y;
	int subchannel_index;
	int subframe_index;
};

#endif /*PACKETS_PACKET_H_*/

