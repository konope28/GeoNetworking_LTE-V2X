#ifndef SIMULATION_VEHICLE_H_
#define SIMULATION_VEHICLE_H_
#include "simulator.h"
#include "environment.h"
#include "physicalresources.h"
#include "LocTEntry.h"
#include "lpv.h"
#include "beaconpacket.h"
#include "lsreqpacket.h"
#include "lsreppacket.h"
#include "gucpacket.h"
#include "tsbpacket.h"
#include "shbpacket.h"
#include "gbcpacket.h"
#include "gacpacket.h"
#include "macbufferelement.h"
#include <string>
#include <vector>
#include <iostream>
#include <queue>
using namespace std;
class Simulator;
struct Environment;
class LocTEntry;
class LPV;
class ShortSensingWindow;
class ReservingVectorElement;
class ResourcesMatrixElement;
class SensingVectorElement;
class MACBufferElement;

class Vehicle
{
public:
	Vehicle(Simulator* sim, Environment* env, const string &gn_addr, double x, double y, double v, double h);
	~Vehicle();

	//---------------------------------------------------------------//
	void GatherTransmissionData(Packet* p);
	void GatherReceptionData(Packet* p);
	//---------------------------------------------------------------//
	void AllocateResources();
	void UpdateSensingWindow();
	bool IsChannelReady(Packet* p);
	void AddToSensingVector(int f_index, int c_index, double loss);
	double CalculatePathLoss(double x, double y);
	//---------------------------------------------------------------//
	void HandlePacket(Packet* p);
	void ReceivePacket(Packet* p);
	Packet* CopyPacket(Packet* p);
	void ScheduleNextTransmission(int time);
	bool NeighbourExists();
	//---------------------------------------------------------------//
	void SendGAC(int dest_x, int dest_y, int radius);
	void HandleGACPacket(Packet* p);
	void TransmitGeoAnycast(Packet* p);
	//---------------------------------------------------------------//
	void SendGBC(int dest_x, int dest_y, int radius);
	void HandleGBCPacket(Packet* p);
	void TransmitGeoBroadcast(Packet* p);
	//---------------------------------------------------------------//
	void SendTSB(int h);
	void HandleTSBPacket(Packet* p);
	//---------------------------------------------------------------//
	void SendSHB();
	void HandleSHBPacket(Packet* p);
	//---------------------------------------------------------------//
	void SendGUC(string req_GN_ADDR);
	void HandleGUCPacket(Packet *p);
	bool IsLSNeeded(string gn_addr);
	//---------------------------------------------------------------//
	void SendLSRequest(Packet* p);
	void TransmitLSRequest(Packet* p);
	void ReTransmitLSRequest(int index);
	void HandleLSRequest(Packet* p);
	//---------------------------------------------------------------//
	void HandleLSReply(Packet* p);
	//---------------------------------------------------------------//
	void TransmitBroadcast(Packet* p);
	void ForwardBroadcast(Packet* p);
	//---------------------------------------------------------------//
	void TransmitUnicast(Packet* p);
	void ForwardUnicast(Packet* p);
	//---------------------------------------------------------------//
	void SendBeacon();
	void HandleBeaconPacket(Packet* bp);
	//---------------------------------------------------------------//
	void FlushLSBuffer(int index);
	void FlushUCBuffer(int index);
	void FlushBCBuffer();
	void FlushMACBuffer();
	void AddToLSBuffer(Packet* p);
	void AddToUCBuffer(Packet* p);
	void AddToBCBuffer(Packet* p);
	void AddToMACBuffer(Packet* p, string a);
	void DropFrontBC();
	void DropFrontMAC();
	//---------------------------------------------------------------//
	string GreedyForwardingAlgorithm(double pos_x, double pos_y, string source_GN_ADDR);
	string GeoBroadcastLineForwardingAlgorithm(Packet* p);
	//---------------------------------------------------------------//
	bool IsPacketDuplicate(Packet* p);
	void AddToDPL(Packet* p);
	//---------------------------------------------------------------//
	void UpdateLocT(LPV* source_lpv, bool is_neighbour);
	void DropLocTEntry(int index);
	//---------------------------------------------------------------//
	string GetGN_ADDR();
	double GetX();
	double GetY();
	double GetV();
	double GetH();
	int GetBeaconTime();
	int GetTransmissionTime();
	int GetResourceAllocationTime();
	vector<vector<ResourcesMatrixElement*>> GetResourcesMatrix();
	vector<LocTEntry*> GetLocT();
	queue<Packet*> GetBC_buffer();
	queue<MACBufferElement*> GetMAC_buffer();
	queue<Packet*> GetReceptionQueue();
	void SetX(double x);
	void SetY(double y);
	void SetV(double v);
	void SetH(double h);
	bool IsActive();
	void Activate();
	void Deactivate();

private:
	Simulator* s;
	Environment* e;
	const string GN_ADDR;
	double coord_X;
	double coord_Y;
	double velocity;
	double heading;
	bool active;
	int next_beacon_time;
	int next_trans_time;
	int next_allocation_time;
	vector<LocTEntry*> LocT;
	queue<MACBufferElement*> MAC_buffer;
	queue<Packet*> BC_buffer;
	queue<Packet*> reception_queue;
	vector<ShortSensingWindow*> sensing_window;
	vector<SensingVectorElement*>reservation_vector;
	vector<SensingVectorElement*> sensing_vector;
	vector<SensingVectorElement*> previous_sensing_vector;
	vector<vector<ResourcesMatrixElement*>> resources_matrix;
};

#endif /*SIMULATION_VEHICLE_H_*/