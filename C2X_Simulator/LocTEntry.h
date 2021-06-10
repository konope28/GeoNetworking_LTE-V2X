#ifndef DATASTRUCTURES_LocTEntry_H_
#define DATASTRUCTURES_LocTEntry_H_
#include "lpv.h"
#include "packet.h"
#include <string>
#include <vector>
#include <queue>

using namespace std;
class LPV;

class LocTEntry
{
public:
	LocTEntry(LPV* l, int lifetime, bool neighbour);
	~LocTEntry();

	LPV* GetLPV();
	int GetLT();
	vector<int> GetDPL();
	string GetGN_ADDR();
	int GetLSCounter();
	bool IsNeighbour();
	bool IsPending();
	void SetNeighbour(bool neighbour);
	void SetPending(bool pending);
	void SetLT(int time);
	void DropFrontLS();
	void DropFrontUC();
	void DecrementLT();
	void IncrementLSCounter();
	void ResetLSCounter();
	void SetLSTimer(int time);
	void ResetLSTimer();
	int GetLSTimer();
	void AddToDPL(int seq_num);
	void DropInDPL();
	void AddToLSBuffer(Packet* p);
	void AddToUCBuffer(Packet* p);
	void DecrementLSBufferTime();
	void DecrementUCBufferTime();
	queue<Packet*> GetUCBuffer();
	queue<Packet*> GetLSBuffer();
	void PopUCBuffer();
	void PopLSBuffer();

private:
	string GN_ADDR;
	LPV* lpv;
	bool IS_PENDING; 
	bool IS_NEIGHBOUR; 
	int LocT_lifetime;
	int LS_counter;
	int LS_timer;
	vector<int> DPL;
	queue<Packet*> UC_buffer;
	queue<Packet*> LS_buffer;
};

#endif /*DATASTRUCTURES_LocTEntry_H_*/