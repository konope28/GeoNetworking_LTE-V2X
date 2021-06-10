#include "simulator.h"
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
using namespace std;

Simulator::Simulator(int stop_t, 
	vector<int> &f_step,
	vector<string> &f_ID,
	vector<double> &f_X,
	vector<double> &f_Y,
	vector<double> &f_V,
	vector<double> &f_H) : kStopTime(stop_t)
{
	//copy vectors from main
	file_step = f_step;
	file_ID = f_ID;
	file_X = f_X;
	file_Y = f_Y;
	file_V = f_V;
	file_H = f_H;

	e = new Environment();
	current_time = 0;
	pos_iter = 0;
}

Simulator::~Simulator(){}

void Simulator::WriteTransmissionFile(int sn, int t_time, string type)
{
	transmission_file.open("transmission_file.csv", ios::app);
	transmission_file << sn << "," << t_time << "," << type << endl;
	transmission_file.close();
}
void Simulator::WriteReceptionFile(int sn, int t_time, int d_time, int num_nodes)
{
	reception_file.open("reception_file.csv", ios::app);
	reception_file << sn << "," << t_time << "," << d_time << "," << num_nodes << endl;
	reception_file.close();
}
int Simulator::GetTime()
{
	return current_time;
}
void Simulator::UpdateVehPosition()
{	//deactivate all vehicles
	for (int i = 0; i < e->vehicles.size(); i++)
	{
		e->vehicles[i]->Deactivate();
	}
	while (pos_iter < file_step.size() && file_step[pos_iter] == current_time / 10)
	{
		if (e->vehicles.empty())
		{
			e->vehicles.push_back(new Vehicle(this, e, file_ID[pos_iter], file_X[pos_iter], file_Y[pos_iter], file_V[pos_iter], file_H[pos_iter]));
		}
		else
		{	//update and activate present
			for (int i = 0; i < e->vehicles.size(); i++)
			{
				if (e->vehicles[i]->GetGN_ADDR() == file_ID[pos_iter])
				{
					e->vehicles[i]->SetX(file_X[pos_iter]);
					e->vehicles[i]->SetY(file_Y[pos_iter]);
					e->vehicles[i]->SetV(file_V[pos_iter]);
					e->vehicles[i]->SetH(file_H[pos_iter]);
					e->vehicles[i]->Activate();
					break;
				}//add new vehicle if reached end of vehicles vector
				else if (i == e->vehicles.size() - 1)
				{
					e->vehicles.push_back(new Vehicle(this, e, file_ID[pos_iter], file_X[pos_iter], file_Y[pos_iter], file_V[pos_iter], file_H[pos_iter]));
				}
			}
		}
		pos_iter++;
	}//delete nonactive (nonpresent)
	if (!e->vehicles.empty())
	{
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (!e->vehicles[i]->IsActive())
			{
				e->vehicles.erase(e->vehicles.begin() + i);
			}
		}
	}
}

void Simulator::Start()
{
	while (current_time < kStopTime * 10) // 1 simulation step = 100ms!
	{
		cout << "Current time - " << current_time << endl;

		//update/add/delete vehicles every 1s
		if (current_time % 10 == 0)
		{
			UpdateVehPosition();
			cout << "VEHICLES POSITION UPDATED" << endl;
		}

		for (int i = 0; i < e->vehicles.size();i++)
		{
			
			if (e->vehicles[i]->GetResourceAllocationTime() == current_time)
			{
				e->vehicles[i]->AllocateResources();
			}
			
		}
		//check lifetimes of LocT and LS/UC/BC/MAC buffers and drop if needed & LS Retransmission
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			while (!e->vehicles[i]->GetMAC_buffer().empty() && e->vehicles[i]->GetMAC_buffer().front()->GetPacket()->GetBasicHeader()->GetLT() == current_time) //MAC buffer
			{
				e->vehicles[i]->DropFrontMAC();
			}
			while (!e->vehicles[i]->GetBC_buffer().empty() && e->vehicles[i]->GetBC_buffer().front()->GetBasicHeader()->GetLT() == current_time) //BC buffer
			{
				e->vehicles[i]->DropFrontBC();
			}
			for (int k = 0; k < e->vehicles[i]->GetLocT().size(); k++)
			{
				if (e->vehicles[i]->GetLocT()[k]->GetLT() == current_time) //LocT
				{
					e->vehicles[i]->DropLocTEntry(k);
					if (k > 0) //decrement k
					{
						k--;
					}			
				}
				if (!e->vehicles[i]->GetLocT().empty())
				{
					while (!e->vehicles[i]->GetLocT()[k]->GetLSBuffer().empty() && e->vehicles[i]->GetLocT()[k]->GetLSBuffer().front()->GetBasicHeader()->GetLT() == current_time) //LS buffer is part of LocT
					{
						e->vehicles[i]->GetLocT()[k]->DropFrontLS();
					}

					while (!e->vehicles[i]->GetLocT()[k]->GetUCBuffer().empty() && e->vehicles[i]->GetLocT()[k]->GetUCBuffer().front()->GetBasicHeader()->GetLT() == current_time) //UC buffer is part of LocT
					{
						e->vehicles[i]->GetLocT()[k]->DropFrontUC();
					}

					//LS retransmission
					while (!e->vehicles[i]->GetLocT()[k]->GetLSBuffer().empty() && e->vehicles[i]->GetLocT()[k]->GetLSTimer() == current_time)
					{
						e->vehicles[i]->ReTransmitLSRequest(k);
					}
				}
			}
		}

		//send beacons
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (e->vehicles[i]->GetBeaconTime() == current_time)
			{
				e->vehicles[i]->SendBeacon();	
			}
		}
		//send packets
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (e->vehicles[i]->GetTransmissionTime() == current_time)
			{
				int packet_type = e->UniRand(1, 100);
				if (packet_type < 41) //GUC
				{
					int dest_index = i;
					while (1) //draw destination of GUC packet
					{
						dest_index = e->UniRand(0, e->vehicles.size() - 1);
						if (dest_index != i)
						{
							break;
						}
					}				
					e->vehicles[i]->SendGUC(e->vehicles[dest_index]->GetGN_ADDR());
				}
				else if (packet_type < 61) //SHB
				{
					e->vehicles[i]->SendSHB();
				}
				else if (packet_type < 81) //TSB
				{
					int hop_num = e->ExpoRandMultiHop();
					e->vehicles[i]->SendTSB(hop_num);
				}
				else if (packet_type < 91) //GBC
				{
					int x = e->UniRand(-80, 120);
					int y = e->UniRand(-50, 60);
					int r = e->UniRand(20, 40);
					e->vehicles[i]->SendGBC(x, y, r);
				}
				else if (packet_type < 101) //GAC
				{
					int x = e->UniRand(-80, 120);
					int y = e->UniRand(-50, 60);
					int r = e->UniRand(20, 40);
					e->vehicles[i]->SendGAC(x, y, r);
				}
				e->vehicles[i]->ScheduleNextTransmission(current_time);
			}
		}
		//Handle packets
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			while (!e->vehicles[i]->GetReceptionQueue().empty() && e->vehicles[i]->GetReceptionQueue().front()->GetReceptionTime() == current_time)
			{
				e->vehicles[i]->HandlePacket(e->vehicles[i]->GetReceptionQueue().front());
			}
		}

		for (int i = 0; i < e->vehicles.size(); i++)
		{
			e->vehicles[i]->UpdateSensingWindow();
		}

		current_time++;
		cout << endl;
	}
	
}




