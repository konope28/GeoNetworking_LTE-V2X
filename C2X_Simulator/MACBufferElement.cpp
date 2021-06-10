#include "macbufferelement.h"

MACBufferElement::MACBufferElement(Packet* p, string nh)
{
	packet = p;
	next_hop = nh;
}
MACBufferElement::~MACBufferElement(){}

Packet* MACBufferElement::GetPacket()
{
	return packet;
}
string MACBufferElement::GetNextHop()
{
	return next_hop;
}