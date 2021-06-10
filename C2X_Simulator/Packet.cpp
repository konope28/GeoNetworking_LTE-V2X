#include "packet.h"

Packet::Packet()
{
	transmission_time = 0;
}

Packet::~Packet(){}

void Packet::ScheduleTransmission(int time)
{
	if (transmission_time == 0)
	{
		transmission_time = time;
	}
	reception_time = time + 1;
}
void Packet::SetTransmissionTime(int time)
{
	transmission_time = time;
}
int Packet::GetReceptionTime()
{
	return reception_time;
}
int Packet::GetTransmissionTime()
{
	return transmission_time;
}
double Packet::GetSenderX()
{
	return sender_X;
}
double Packet::GetSenderY()
{
	return sender_Y;
}
void Packet::SetSenderX(double x)
{
	sender_X = x;
}
void Packet::SetSenderY(double y)
{
	sender_Y = y;
}
int Packet::GetSubchannelIndex()
{
	return subchannel_index;
}
int Packet::GetSubframeIndex()
{
	return subframe_index;
}
void Packet::SetSubchannelIndex(int c)
{
	subchannel_index = c;
}
void Packet::SetSubframeIndex(int f)
{
	subframe_index = f;
}