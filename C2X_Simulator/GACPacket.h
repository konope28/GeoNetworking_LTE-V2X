#ifndef PACKETS_GACPACKET_H_
#define PACKETS_GACPACKET_H_
#include "packet.h"
#include "gacheader.h"
class GACHeader;

class GACPacket : public Packet
{
public:
	GACPacket(LPV* so_pv, int dest_x, int dest_y, int radious, int current_time);
	~GACPacket();
	BasicHeader* GetBasicHeader() override;
	CommonHeader* GetCommonHeader() override;
	LPV* GetSOPV() override;
	int GetSN() override;
	int GetDestX() override;
	int GetDestY() override;
	int GetRadius() override;
	string GetSenderGN_ADDR() override;
	void SetSenderGN_ADDR(string gn_addr);
	void SetSN(int sn) override;

	virtual string GetReq_GN_ADDR() { return 0; };
	virtual void UpdateDEPV(double x, double y) {};
	virtual SPV* GetDEPV() { return 0; };

private:
	GACHeader* gac_head;
	string sender_GN_ADDR;
	int payload;
};

#endif /*PACKETS_GACPACKET_H_*/
