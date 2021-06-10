#ifndef PACKETS_BEACONPACKET_H_
#define PACKETS_BEACONPACKET_H_
#include "packet.h"
#include "beaconheader.h"

class LPV;

class BeaconPacket : public Packet
{
public:
	BeaconPacket(LPV* lpv, int current_time);
	~BeaconPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	int GetSN() override;

	virtual SPV* GetDEPV() { return 0; };
	virtual string GetReq_GN_ADDR() { return 0; };
	virtual void UpdateDEPV(double x, double y) {};
	virtual void SetSN(int sn) {};
	virtual int GetDestX() { return 0; };
	virtual int GetDestY() { return 0; };
	virtual int GetRadius() { return 0; };
	virtual string GetSenderGN_ADDR() { return 0; };
	virtual void SetSenderGN_ADDR(string gn) {};

private:
	BeaconHeader* beacon_head;
};

#endif /*PACKETS_BEACONPACKET_H_*/

