#ifndef DATASTRUCTURES_MACBUFFERELEMENT_H_
#define DATASTRUCTURES_MACBUFFERELEMENT_H_
#include <string>
#include "packet.h"
using namespace std;


class MACBufferElement
{
public:
	MACBufferElement(Packet* packet, string next_hop);
	~MACBufferElement();
	Packet* GetPacket();
	string GetNextHop();

private:
	Packet* packet;
	string next_hop;
};

#endif /*DATASTRUCTURES_MACBUFFERELEMENT_H_*/#
