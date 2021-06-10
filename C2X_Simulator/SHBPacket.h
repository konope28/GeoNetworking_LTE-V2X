#ifndef PACKETS_SHBPACKET_H_
#define PACKETS_SHBPACKET_H_
#include "packet.h"
#include "shbheader.h"

class SHBPacket : public Packet
{
public:
	SHBPacket(LPV* so_pv, int current_time);
	~SHBPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	int GetSN() override;
	
	virtual SPV* GetDEPV() { return 0; };
	virtual void UpdateDEPV(double x, double y) {};
	virtual string GetReq_GN_ADDR() { return 0; };
	virtual void SetSN(int sn) {}; 
	virtual int GetDestX() { return 0; };
	virtual int GetDestY() { return 0; };
	virtual int GetRadius() { return 0; };
	virtual string GetSenderGN_ADDR() { return 0; };
	virtual void SetSenderGN_ADDR(string gn) {};

private:
	SHBHeader* shb_head;
	int payload;
};

#endif /*PACKETS_SHBPACKET_H_*/
