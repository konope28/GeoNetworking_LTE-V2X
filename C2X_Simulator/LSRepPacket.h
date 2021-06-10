#ifndef PACKETS_LSREPPACKET_H_
#define PACKETS_LSREPPACKET_H_
#include "packet.h"
#include "lsrepheader.h"

class LSRepPacket : public Packet
{
public:
	LSRepPacket(LPV* so_pv, SPV* de_pv, int current_time);
	~LSRepPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	SPV* GetDEPV() override;
	int GetSN() override;
	void UpdateDEPV(double x, double y) override;

	virtual string GetReq_GN_ADDR() { return 0; };
	virtual void SetSN(int sn) {};
	virtual int GetDestX() { return 0; };
	virtual int GetDestY() { return 0; };
	virtual int GetRadius() { return 0; };
	virtual string GetSenderGN_ADDR() { return 0; };
	virtual void SetSenderGN_ADDR(string gn) {};

private:
	LSRepHeader* ls_rep_head;
	int payload;
};

#endif /*PACKETS_LSREPPACKET_H_*/
