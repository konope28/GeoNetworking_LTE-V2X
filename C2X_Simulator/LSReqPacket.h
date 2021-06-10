#ifndef PACKETS_LSREQPACKET_H_
#define PACKETS_LSREQPACKET_H_
#include "packet.h"
#include "lsreqheader.h"


class LSReqPacket : public Packet
{
public:
	LSReqPacket(LPV* so_pv, string gn_addr, int current_time);
	~LSReqPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	int GetSN() override;
	string GetReq_GN_ADDR() override;
	void SetSN(int sn) override;

	virtual SPV* GetDEPV() { return 0; };
	virtual void UpdateDEPV(double x, double y) {};
	virtual int GetDestX() { return 0; };
	virtual int GetDestY() { return 0; };
	virtual int GetRadius() { return 0; };
	virtual string GetSenderGN_ADDR() { return 0; };
	virtual void SetSenderGN_ADDR(string gn) {};

private:
	LSReqHeader* ls_req_head;
};

#endif /*PACKETS_LSREQPACKET_H_*/
