#include "LocTEntry.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


LocTEntry::LocTEntry(LPV* l, int t, bool n)
{
	GN_ADDR = l->GetGN_ADDR();
	lpv = l;
	IS_NEIGHBOUR = n;
	IS_PENDING = false;
	LocT_lifetime = t + 200;	//20s by default
	LS_counter = 0;
	
}
LocTEntry::~LocTEntry(){}


LPV* LocTEntry::GetLPV()
{
	return lpv;
}
int LocTEntry::GetLT()
{
	return LocT_lifetime;
}
vector<int> LocTEntry::GetDPL()
{
	return DPL;
}
string LocTEntry::GetGN_ADDR()
{
	return GN_ADDR;
}
int LocTEntry::GetLSCounter()
{
	return LS_counter;
}
bool LocTEntry::IsNeighbour()
{
	return IS_NEIGHBOUR;
}
bool LocTEntry::IsPending()
{
	return IS_PENDING;
}
void LocTEntry::SetNeighbour(bool n)
{
	IS_NEIGHBOUR = n;
}
void LocTEntry::SetPending(bool p)
{
	IS_PENDING = p;
}
void LocTEntry::DecrementLT()
{
	LocT_lifetime--;
}
void LocTEntry::IncrementLSCounter()
{
	LS_counter++;
}
void LocTEntry::ResetLSCounter()
{
	LS_counter = 0;
}
void LocTEntry::AddToDPL(int sn)
{
	DPL.push_back(sn);
}
void LocTEntry::DropInDPL()
{
	DPL.erase(DPL.begin());
}
void LocTEntry::AddToLSBuffer(Packet* packet)
{
	LS_buffer.push(packet);
	if (LS_buffer.size() > 10) //not defined in standard - assumption
	{
		LS_buffer.pop();
	}
}
void LocTEntry::AddToUCBuffer(Packet* packet)
{
	UC_buffer.push(packet);
	if (UC_buffer.size() > 10) //not defined in standard - assumption
	{
		UC_buffer.pop();
	}
}
queue<Packet*> LocTEntry::GetUCBuffer()
{
	return UC_buffer;
}
queue<Packet*> LocTEntry::GetLSBuffer()
{
	return LS_buffer;
}
void LocTEntry::PopUCBuffer()
{
	UC_buffer.pop();
}
void LocTEntry::PopLSBuffer()
{
	LS_buffer.pop();
}
void LocTEntry::DecrementLSBufferTime()
{
	for (int i = 0; i < LS_buffer.size(); i++)
	{
		Packet* p = LS_buffer.front();
		p->GetBasicHeader()->DecrementLT();
		if (p->GetBasicHeader()->GetLT() > 0)
		{
			LS_buffer.push(p);
		}
		LS_buffer.pop();
	}
}
void LocTEntry::DecrementUCBufferTime()
{
	for (int i = 0; i < UC_buffer.size(); i++)
	{
		Packet* p = UC_buffer.front();
		p->GetBasicHeader()->DecrementLT();
		if (p->GetBasicHeader()->GetLT() > 0)
		{
			UC_buffer.push(p);
		}
		UC_buffer.pop();
	}
}
void LocTEntry::SetLT(int time)
{
	LocT_lifetime = time + 200;
}
void LocTEntry::DropFrontLS()
{
	LS_buffer.pop();
}
void LocTEntry::DropFrontUC()
{
	UC_buffer.pop();
}
void LocTEntry::SetLSTimer(int time)
{
	LS_timer = time;
}
void LocTEntry::ResetLSTimer()
{
	LS_timer = 0;
}
int LocTEntry::GetLSTimer()
{
	return LS_timer;
}
