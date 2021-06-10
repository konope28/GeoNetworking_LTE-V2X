#ifndef PACKETS_TSBPACKET_H_
#define PACKETS_TSBPACKET_H_
#include "packet.h"
#include "tsbheader.h"

class TSBPacket : public Packet
{
public:
	TSBPacket(LPV* so_pv, int hops, int current_time);
	~TSBPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	int GetSN() override;
	void SetSN(int sn) override;

	virtual SPV* GetDEPV() { return 0; };
	virtual void UpdateDEPV(double x, double y) {};
	virtual string GetReq_GN_ADDR() { return 0; };
	virtual int GetDestX() { return 0; };
	virtual int GetDestY() { return 0; };
	virtual int GetRadius() { return 0; };
	virtual string GetSenderGN_ADDR() { return 0; };
	virtual void SetSenderGN_ADDR(string gn) {};

private:
	TSBHeader* tsb_head;
	int payload;
};

#endif /*PACKETS_TSBPACKET_H_*/
