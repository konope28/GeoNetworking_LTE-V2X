#include "vehicle.h"
#include <math.h>


Vehicle::Vehicle(Simulator* sim, Environment* env, const string &gn_addr, double x, double y, double v, double h) : GN_ADDR(gn_addr)
{
	s = sim;
	e = env;
	coord_X = x;
	coord_Y = y;
	velocity = v;
	heading = h;
	active = true;
	next_beacon_time = s->GetTime();
	next_trans_time = s->GetTime() + e->ExpoRandTransmission();
	next_allocation_time = s->GetTime();
	
	for (int i = 0; i < 10; i++) //create sensing window of length 1s
	{
		sensing_window.push_back(new ShortSensingWindow());
	}

	for (int k = 0; k < 100; k++) //create matrix of resources of length 100ms
	{
		vector<ResourcesMatrixElement*> temp_vec;
		for (int j = 0; j < 5; j++)
		{
			temp_vec.push_back(new ResourcesMatrixElement());
		}
		resources_matrix.push_back(temp_vec);
	}
}
Vehicle::~Vehicle(){}


//-----DATA GATHERING--------------------------------------------------------------------------------------------------------------------//

void Vehicle::GatherTransmissionData(Packet* packet)
{
	string type;
	switch (packet->GetCommonHeader()->GetHT())
	{
		case 1: //beacon
		{
			type = "beacon";
			break;
		}
		case 2: //GeoUnicast
		{
			type = "GUC";
			break;
		}
		case 3: //GeoAnycast
		{
			type = "GAC";
			break;
		}
		case 4: //GeoBroadcast
		{
			type = "GBC";
			break;
		}
		case 5: //Topologically-scoped broadcast
		{
			if (packet->GetCommonHeader()->GetHST() == 0) //SHB
			{
				type = "SHB";
			}
			else if (packet->GetCommonHeader()->GetHST() == 1) //TSB
			{
				type = "TSB";
			}
			break;
		}
		case 6: //Location service
		{
			if (packet->GetCommonHeader()->GetHST() == 0) //REQUEST
			{
				type = "LS Request";
			}
			else if (packet->GetCommonHeader()->GetHST() == 1) //REPLY
			{
				type = "LS Reply";
			}
			break;
		}
		default:
			break;
	}
	s->WriteTransmissionFile(packet->GetSN(), s->GetTime(), type);
}
void Vehicle::GatherReceptionData(Packet* packet)
{
	s->WriteReceptionFile(packet->GetSN(), packet->GetTransmissionTime(), s->GetTime(), e->vehicles.size());
}

//-----RESOURCES MANAGEMENT--------------------------------------------------------------------------------------------------------------//

void Vehicle::AllocateResources()
{
	if (reservation_vector.empty() || e->UniRand(1, 10) > 6) //if there are some resources allocated there is 60% chances to keep them
	{
		int threshold = 130; //initial threshlod 130dB
		int previously_allocated = reservation_vector.size();
		while (true)
		{
			while (!reservation_vector.size() == 0) //clear reservation vector
			{
				reservation_vector.erase(reservation_vector.begin());
			}

			for (int a = 0; a < 100; a++) //subframes
			{
				for (int b = 0; b < 5; b++) //subchannels
				{
					resources_matrix[a][b]->SetUsed(false); //clear resources matrix

					reservation_vector.push_back(new SensingVectorElement(a, b, 1000)); //all resources are available for reservation at the begining	

					for (int i = 0; i < 10; i++) //'short windows'
					{
						if (sensing_window[i]->GetPathLoss(a, b) < threshold)
						{
							reservation_vector.pop_back(); //delete reservation for resources used by others and with small path loss
							break;
						}
						else if (sensing_window[i]->GetPathLoss(a, b) < reservation_vector.back()->GetPathLoss())
						{
							reservation_vector.back()->SetPathLoss(sensing_window[i]->GetPathLoss(a, b)); //update pathloss in reservation vector
						}
					}
				}
			}

			if (reservation_vector.size() >= 100) //check if there is at least 20% of resources available
			{
				if (reservation_vector.size() != 100) //limit resources reservation to 20% of available only
				{
					vector<SensingVectorElement*> temp_vector; //temp_vector stores 20& of resources
					while (temp_vector.size() < 100)
					{
						int max_loss_index = 0;
						for (int i = 0; i < reservation_vector.size(); i++)
						{
							if (reservation_vector[i]->GetPathLoss() > reservation_vector[max_loss_index]->GetPathLoss())
							{
								max_loss_index = i;
							}
						}
						temp_vector.push_back(reservation_vector[max_loss_index]);
						reservation_vector.erase(reservation_vector.begin() + max_loss_index);
					}
					reservation_vector = temp_vector;
				}

				int allocation_counter = 0;
				if (MAC_buffer.size() != 0)//not empty MAC Buffer
				{
					allocation_counter = previously_allocated + MAC_buffer.size(); //allocate more resources
				}
				else
				{
					allocation_counter = previously_allocated - 1; //allocate less resources
				}

				if (allocation_counter < 2)
				{
					allocation_counter = 2; //minimum is 2 subframes/subchannels
				}

				vector<SensingVectorElement*> temp_vector; //temp_vector stores resources that are allocated
				for (int i = 0; i < allocation_counter; i++) //draw resources to be allocated and mark as reserved
				{
					int rand = e->UniRand(0, reservation_vector.size() - 1);

					temp_vector.push_back(reservation_vector[rand]);
					reservation_vector.erase(reservation_vector.begin() + rand);
				}
				reservation_vector = temp_vector;
				break;
			}
			else //decrease threshold by 3 dB and start again
			{
				threshold -= 3;
			}
		}
	}
	else //keep resources
	{
		for (int a = 0; a < 100; a++)
		{
			for (int b = 0; b < 5; b++)
			{
				resources_matrix[a][b]->SetUsed(false); //clear resources matrix
			}
		}

		for (int i = 0; i < reservation_vector.size(); i++)
		{
			int subframe = reservation_vector[i]->GetSubframeIndex();
			int subchannel = reservation_vector[i]->GetSubchannelIndex();

			reservation_vector[i]->SetPathLoss(1000.0); //reset pathloss in reservation vector
			for (int k = 0; k < 10; k++)
			{
				if (sensing_window[k]->GetPathLoss(subframe, subchannel) > reservation_vector[i]->GetPathLoss())
				{
					reservation_vector[i]->SetPathLoss(sensing_window[k]->GetPathLoss(subframe, subchannel));
				}
			}
		}
	}

	FlushMACBuffer();
	next_allocation_time = s->GetTime() + e->UniRand(6, 12);
}
void Vehicle::UpdateSensingWindow()
{
	ShortSensingWindow* new_window = new ShortSensingWindow();
	previous_sensing_vector = sensing_vector; //store previous sensing vector

	while (!sensing_vector.empty())
	{
		int index = sensing_vector.size() - 1;
		double loss = sensing_vector[index]->GetPathLoss();

		int sf = sensing_vector[index]->GetSubframeIndex();
		int sc = sensing_vector[index]->GetSubchannelIndex();


		if (loss < new_window->GetPathLoss(sf, sc)) //update path loss if it is lower than sensed previously
		{
			new_window->SetPathLoss(sf, sc, loss);
		}
		sensing_vector.pop_back();
	}

	//push and pop from sensing window
	sensing_window.push_back(new_window);
	sensing_window.erase(sensing_window.begin());
}
bool Vehicle::IsChannelReady(Packet* packet)
{
	for (int i = 0; i < reservation_vector.size(); i++)
	{
		int subframe = reservation_vector[i]->GetSubframeIndex();
		int subchannel = reservation_vector[i]->GetSubchannelIndex();

		//if these resources were not used before
		if (!resources_matrix[subframe][subchannel]->IsUsed())
		{
			resources_matrix[subframe][subchannel]->SetUsed(true);
			packet->SetSubframeIndex(subframe);
			packet->SetSubchannelIndex(subchannel);
			packet->SetSenderX(coord_X);
			packet->SetSenderY(coord_Y);

			//all other vehicles are sensing
			for (int i = 0; i < e->vehicles.size(); i++)
			{
				if (e->vehicles[i]->GetGN_ADDR() != GN_ADDR)
				{
					e->vehicles[i]->AddToSensingVector(subframe, subchannel, CalculatePathLoss(e->vehicles[i]->GetX(), e->vehicles[i]->GetY()));
				}	//all vehicles notice transmission, no matter the power.
			}
			return true;
		}
	}
	return false;
}
void Vehicle::AddToSensingVector(int subframe, int subchannel, double loss)
{
	sensing_vector.push_back(new SensingVectorElement(subframe, subchannel, loss));
}
double Vehicle::CalculatePathLoss(double x, double y)
{
	double distance = sqrt(pow(x - coord_X, 2) + pow(y - coord_Y, 2)); //in meters
	return  2.75 * 10 * log10(distance) + 43.9 + e->NormalRand();
}

//-----GENERAL PACKET HANDLING-----------------------------------------------------------------------------------------------------------//

void Vehicle::HandlePacket(Packet* packet)
{
	reception_queue.pop();
	if (packet->GetSOPV()->GetGN_ADDR() != GN_ADDR) //do not receive if you are the sender
	{
		bool packet_received = false;
		double loss = CalculatePathLoss(packet->GetSenderX(), packet->GetSenderY());

		if (loss < 113) //check if packet is possible to receive in terms of power (threshold set to 113dB)
		{
			packet_received = true;
			int subframe = packet->GetSubframeIndex();
			int subchannel = packet->GetSubchannelIndex();

			double I = pow(10, -11.3) - loss; //interference = -113dB in linear scale; loss is subctracted as it will be added in a loop below

			for (int i = 0; i < previous_sensing_vector.size(); i++)
			{
				//if there was any (including this one) transmission in this subframe and subchannel in previous step
				if (previous_sensing_vector[i]->GetSubframeIndex() == subframe && previous_sensing_vector[i]->GetSubchannelIndex() == subchannel)
				{
					I = I + pow(10, -previous_sensing_vector[i]->GetPathLoss() / 10.0);
				}
			}
			double sinr = -loss - 10 * log10(I);
			if (sinr <= 0)
			{
				packet_received = false;
			}

			if (packet_received)
			{
				for (int i = 0; i < 5; i++) //check if this vehicle transmitts anything in the same subframe as the packet is received
				{
					if (resources_matrix[subframe][i]->IsUsed())
					{
						packet_received = false;
						break;
					}
				}
			}
		}

		if (packet_received)
		{
			switch (packet->GetCommonHeader()->GetHT())
			{
				case 1: //beacon
				{
					HandleBeaconPacket(packet);
					break;
				}
				case 2: //GeoUnicast
				{
					HandleGUCPacket(packet);
					break;
				}
				case 3: //GeoAnycast
				{
					HandleGACPacket(packet);
					break;
				}
				case 4: //GeoBroadcast
				{
					HandleGBCPacket(packet);
					break;
				}
				case 5: //Topologically-scoped broadcast
				{
					if (packet->GetCommonHeader()->GetHST() == 0) //SHB
					{
						HandleSHBPacket(packet);
					}
					else if (packet->GetCommonHeader()->GetHST() == 1) //TSB
					{
						HandleTSBPacket(packet);
					}
					break;
				}
				case 6: //Location service
				{
					if (packet->GetCommonHeader()->GetHST() == 0) //REQUEST
					{
						HandleLSRequest(packet);
					}
					else if (packet->GetCommonHeader()->GetHST() == 1) //REPLY
					{
						HandleLSReply(packet);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}
void Vehicle::ReceivePacket(Packet* packet)
{
	reception_queue.push(packet);
}
Packet* Vehicle::CopyPacket(Packet* packet)
{
	Packet* new_packet = NULL;
	switch (packet->GetCommonHeader()->GetHT())
	{
		case 2: //GeoUnicast
		{
			new_packet = new GUCPacket(packet->GetSOPV(), packet->GetDEPV(), packet->GetBasicHeader()->GetLT());
			break;
		}
		case 3: //GeoAnycast
		{
			new_packet = new GACPacket(packet->GetSOPV(), packet->GetDestX(), packet->GetDestY(), packet->GetRadius(), packet->GetBasicHeader()->GetLT());
			new_packet->SetSenderGN_ADDR(packet->GetSenderGN_ADDR());
			break;
		}
		case 4: //GeoBroadcast
		{
			new_packet = new GBCPacket(packet->GetSOPV(), packet->GetDestX(), packet->GetDestY(), packet->GetRadius(), packet->GetBasicHeader()->GetLT());
			new_packet->SetSenderGN_ADDR(packet->GetSenderGN_ADDR());
			break;
		}
		case 5: //TSB
		{
			new_packet = new TSBPacket(packet->GetSOPV(), packet->GetBasicHeader()->GetRHL(), packet->GetBasicHeader()->GetLT());
			break;
		}
		case 6: //Location service
		{
			if (packet->GetCommonHeader()->GetHST() == 0) //Request
			{
				new_packet = new LSReqPacket(packet->GetSOPV(), packet->GetReq_GN_ADDR(), packet->GetBasicHeader()->GetLT());
				break;
			}
			else if (packet->GetCommonHeader()->GetHST() == 1) //Reply
			{
				new_packet = new LSRepPacket(packet->GetSOPV(), packet->GetDEPV(), packet->GetBasicHeader()->GetLT());
				break;
			}
		}
		default:
			break;
	}
	new_packet->GetBasicHeader()->SetLT(packet->GetBasicHeader()->GetLT());		//update lifetime
	new_packet->GetBasicHeader()->SetRHL(packet->GetBasicHeader()->GetRHL());	//update rhl
	new_packet->SetSN(packet->GetSN());											//update sequence number
	new_packet->SetTransmissionTime(packet->GetTransmissionTime());				//update transmission time
	return new_packet;
}
void Vehicle::ScheduleNextTransmission(int time)
{
	next_trans_time = time + e->ExpoRandTransmission();
}
bool Vehicle::NeighbourExists()
{
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->IsNeighbour())
		{
			return true;
		}
	}
	return false;
}

//-----GEO ANYCAST-----------------------------------------------------------------------------------------------------------------------//

void Vehicle::SendGAC(int x, int y, int r)
{
	Packet* gac_packet = new GACPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), x, y, r, s->GetTime());
	GatherTransmissionData(gac_packet);
	cout << "[GEO ANYCAST] " << GN_ADDR << " wants to transmit GAC Packet, SN: " << gac_packet->GetSN() << " to (" << x << "," << y << "), with radius = " << r << endl;
	gac_packet->SetSenderGN_ADDR(GN_ADDR);
	TransmitGeoAnycast(gac_packet);
}
void Vehicle::HandleGACPacket(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		
		cout << "[GEO ANYCAST] " << GN_ADDR << "	receives GAC Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to (" << packet->GetDestX() << "," << packet->GetDestY() << ")" << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);

		for (int i = 0; i < LocT.size(); i++)
		{
			if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
			{
				FlushUCBuffer(i);
				if (LocT[i]->IsPending())
				{
					LocT[i]->SetPending(false);
					FlushLSBuffer(i);
				}
				break;
			}
		}
		FlushBCBuffer();
		
		Packet* forwarded_packet = CopyPacket(packet);
		double distance = sqrt(pow(forwarded_packet->GetDestX() - coord_X, 2) + pow(forwarded_packet->GetDestY() - coord_Y, 2));
		if (distance > forwarded_packet->GetRadius()) //vehicle is outside geographical area
		{
			forwarded_packet->GetBasicHeader()->DecrementRHL();
			if (forwarded_packet->GetBasicHeader()->GetRHL() > 0)
			{
				string next_hop_gn_addr = GreedyForwardingAlgorithm(forwarded_packet->GetDestX(), forwarded_packet->GetDestY(), packet->GetSOPV()->GetGN_ADDR()); //forwarding algorithm
				forwarded_packet->SetSenderGN_ADDR(GN_ADDR);
				if (next_hop_gn_addr != "0")
				{
					if (IsChannelReady(forwarded_packet)) //choose channel
					{
						forwarded_packet->ScheduleTransmission(s->GetTime());
						for (int i = 0; i < e->vehicles.size(); i++)
						{
							if (e->vehicles[i]->GetGN_ADDR() == next_hop_gn_addr) //forward packet
							{
								cout << "[GEO ANYCAST] " << "Packet, SN: " << forwarded_packet->GetSN() << " forwarded from " << GN_ADDR << " to " << e->vehicles[i]->GetGN_ADDR() << endl;
								e->vehicles[i]->ReceivePacket(forwarded_packet);
								break;
							}
						}
					}
					else //Add to MAC Buffer
					{
						cout << "[GEO ANYCAST] " << " GAC Packet, SN: " << forwarded_packet->GetSN() << " added to MAC buffer " << endl;
						AddToMACBuffer(forwarded_packet, next_hop_gn_addr);
					}
				}
				else //Add to BC Buffer
				{
					cout << "[GEO ANYCAST] " << GN_ADDR << "  did not found a forwarder (packet added to BC buffer)" << endl;
					AddToBCBuffer(forwarded_packet);
				}
			}
		}
		else
		{
			GatherReceptionData(packet);
			cout << "[GEO ANYCAST] " << GN_ADDR << "	is inside area and receives GAC Packet, SN: " << packet->GetSN() << " from " << forwarded_packet->GetSOPV()->GetGN_ADDR() << endl;
		}
	}
	else
	{
		cout << "[GEO ANYCAST] " << GN_ADDR << "	receives duplicated GAC Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << endl;
	}
}
void Vehicle::TransmitGeoAnycast(Packet* packet)
{
	if (NeighbourExists())
	{

		string next_hop_gn_addr = GeoBroadcastLineForwardingAlgorithm(packet);
		packet->SetSenderGN_ADDR(GN_ADDR);
		if (next_hop_gn_addr == "-1") //discard packet
		{
			cout << "[GEO ANYCAST] " << " GAC Packet, SN: " << packet->GetSN() << " is discarded" << endl;
			//do not do anything
		}
		else if (next_hop_gn_addr == "0") //add packet to BC buffer
		{
			cout << "[GEO ANYCAST] " << " GAC Packet, SN: " << packet->GetSN() << " is added to the BC buffer " << endl;
			AddToBCBuffer(packet);
		}
		else if (next_hop_gn_addr == "BCAST") //broadcast packet
		{
			cout << "[GEO ANYCAST] " << " GAC Packet, SN: " << packet->GetSN() << " is going to be transmitted as broadcast " << endl;
			Packet* forwarded_packet = CopyPacket(packet);
			TransmitBroadcast(forwarded_packet);
		}
		else //forwarder found
		{
			Packet* forwarded_packet = CopyPacket(packet);
			if (IsChannelReady(forwarded_packet))
			{
				forwarded_packet->ScheduleTransmission(s->GetTime());
				for (int i = 0; i < e->vehicles.size(); i++)
				{
					if (e->vehicles[i]->GetGN_ADDR() == next_hop_gn_addr)
					{
						cout << "[GEO ANYCAST] " << "GAC packet, SN: " << packet->GetSN() << " forwarded from " << GN_ADDR << " to " << e->vehicles[i]->GetGN_ADDR() << endl;			
						e->vehicles[i]->ReceivePacket(forwarded_packet);
						break;
					}
				}
			}
			else //Add to MAC Buffer
			{
				cout << "[GEO ANYCAST] " << " GAC Packet, SN: " << packet->GetSN() << " added to MAC buffer " << endl; 
				AddToMACBuffer(forwarded_packet, next_hop_gn_addr);
			}
		}
	}
	else
	{
		cout << "[GEO ANYCAST] " << GN_ADDR << "  has no suitable neighbour (packet added to BC buffer)" << endl;
		AddToBCBuffer(packet);
	}
}

//-----GEO BROADCAST---------------------------------------------------------------------------------------------------------------------//

void Vehicle::SendGBC(int x, int y, int r)
{
	Packet* gbc_packet = new GBCPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), x, y, r, s->GetTime());
	GatherTransmissionData(gbc_packet);
	gbc_packet->SetSenderGN_ADDR(GN_ADDR);
	cout << "[GEO BROADCAST] " << GN_ADDR << "	wants to transmit GBC Packet, SN: " << gbc_packet->GetSN() << " to (" << x << "," << y << "), with radius = " << r << endl;

	TransmitGeoBroadcast(gbc_packet);
}
void Vehicle::HandleGBCPacket(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		
		cout << "[GEO BROADCAST] " << GN_ADDR << "	receives GBC Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to (" << packet->GetDestX() << "," << packet->GetDestY() << ")" << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);
		

		for (int i = 0; i < LocT.size(); i++)
		{
			if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
			{
				FlushUCBuffer(i);
				if (LocT[i]->IsPending())
				{
					LocT[i]->SetPending(false);
					FlushLSBuffer(i);
				}
				break;
			}
		}
		FlushBCBuffer();

		double distance = sqrt(pow(packet->GetDestX() - coord_X, 2) + pow(packet->GetDestY() - coord_Y, 2));
		if (distance <= packet->GetRadius()) //vehicle is inside geographical area
		{
			GatherReceptionData(packet);
		}

		packet->GetBasicHeader()->DecrementRHL();
		if (packet->GetBasicHeader()->GetRHL() >= 0)
		{
			TransmitGeoBroadcast(packet);
		}	
	}
	else
	{
		cout << "[GEO BROADCAST] " << GN_ADDR << "	receives duplicated GBC Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << endl;
	}
}
void Vehicle::TransmitGeoBroadcast(Packet* packet)
{
	if (NeighbourExists())
	{
		string next_hop_gn_addr = GeoBroadcastLineForwardingAlgorithm(packet);
		packet->SetSenderGN_ADDR(GN_ADDR);
		if (next_hop_gn_addr == "-1") //discard packet
		{
			cout << "[GEO BROADCAST] " << " GBC Packet, SN: " << packet->GetSN() << " is discarded" << endl;
			//do not do anything
		}
		else if (next_hop_gn_addr == "0") //add packet to BC buffer
		{
			cout << "[GEO BROADCAST] " << " GBC Packet, SN: " << packet->GetSN() << " added to the BC buffer" << endl;
			AddToBCBuffer(packet);
		}
		else if (next_hop_gn_addr == "BCAST") //broadcast packet
		{
			cout << "[GEO BROADCAST] " << " GBC Packet, SN: " << packet->GetSN() << " is going to be transmitted as broadcast" << endl;
			Packet* forwarded_packet = CopyPacket(packet);
			TransmitBroadcast(forwarded_packet);
		}
		else //forwarder found
		{
			Packet* forwarded_packet = CopyPacket(packet);
			if (IsChannelReady(forwarded_packet))
			{
				forwarded_packet->ScheduleTransmission(s->GetTime());
				for (int i = 0; i < e->vehicles.size(); i++)
				{
					if (e->vehicles[i]->GetGN_ADDR() == next_hop_gn_addr)
					{
						cout << "[GEO BROADCAST] " << "GBC packet, SN: " << packet->GetSN() << " forwarded from " << GN_ADDR << " to " << e->vehicles[i]->GetGN_ADDR() << endl;					
						e->vehicles[i]->ReceivePacket(forwarded_packet);
						break;
					}
				}
			}
			else //Add to MAC Buffer
			{
				cout << "[GEO BROADCAST] " << " GBC Packet, SN: " << packet->GetSN() << " added to the MAC buffer" << endl;
				AddToMACBuffer(forwarded_packet, next_hop_gn_addr);
			}
		}
	}
	else
	{
		cout << "[GEO BROADCAST] " << GN_ADDR << "  has no suitable neighbour (packet added to BC buffer)" << endl;
		AddToBCBuffer(packet);
	}
	
}

//-----TOPOLOGICALLY SCOPED BROADCAST----------------------------------------------------------------------------------------------------//

void Vehicle::SendTSB(int hop_num)
{
	Packet* tsb_packet = new TSBPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), hop_num, s->GetTime());
	GatherTransmissionData(tsb_packet);
	cout << "[TSB] " << GN_ADDR << " wants to transmit TSB, SN: "<< tsb_packet->GetSN() <<" with " << hop_num << " forwardings" << endl;

	if (NeighbourExists())
	{
		TransmitBroadcast(tsb_packet);
	}
	else
	{
		cout << "[TSB] " << GN_ADDR << "  has no suitable neighbour (packet added to BC buffer)" << endl;
		AddToBCBuffer(tsb_packet);
	}
	
}
void Vehicle::HandleTSBPacket(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		GatherReceptionData(packet);
		cout << "[TSB] " << GN_ADDR << " receives TSB Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR()  << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);

		ForwardBroadcast(packet);
	}
	else
	{
		cout << "[TSB] " << GN_ADDR << " receives duplicated TSB Packet, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << endl;
	}

}

//-----SINGLE-HOP BROADCAST--------------------------------------------------------------------------------------------------------------//

void Vehicle::SendSHB()
{
	Packet* shb_packet = new SHBPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), s->GetTime());
	GatherTransmissionData(shb_packet);
	cout << "[SHB] " << GN_ADDR << " wants to transmit SHB Packet" << endl;

	if (NeighbourExists())
	{
		TransmitBroadcast(shb_packet);
		next_beacon_time = s->GetTime() + e->UniRand(23, 37);
	}
	else
	{
		cout << "[SHB] " << GN_ADDR << " has no suitable neighbour (packet added to BC buffer)" << endl;
		AddToBCBuffer(shb_packet);
	}
}
void Vehicle::HandleSHBPacket(Packet* packet)
{
	GatherReceptionData(packet);
	cout << "[SHB] " << GN_ADDR << " receives SHB packet from " << packet->GetSOPV()->GetGN_ADDR() << endl;
	UpdateLocT(packet->GetSOPV(), true);
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
		{
			LocT[i]->SetNeighbour(true);
			FlushUCBuffer(i);
			if (LocT[i]->IsPending())
			{
				LocT[i]->SetPending(false);
				FlushLSBuffer(i);
			}
			break;
		}
	}
	FlushBCBuffer();
}

//-----GEOUNICAST------------------------------------------------------------------------------------------------------------------------//

void Vehicle::SendGUC(string req_GN_ADDR)
{
	Packet* guc_packet = new GUCPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), new SPV(req_GN_ADDR, NULL, NULL, s->GetTime()), s->GetTime());
	GatherTransmissionData(guc_packet);
	cout << "[GUC] " << GN_ADDR << " wants to transmit GUC, SN: " << guc_packet->GetSN() << " to " << req_GN_ADDR << endl;

	if (IsLSNeeded(req_GN_ADDR))
	{
		SendLSRequest(guc_packet); // Send LS Request
	}
	else
	{
		for (int i = 0; i < LocT.size(); i++)
		{
			if (LocT[i]->GetLPV()->GetGN_ADDR() == req_GN_ADDR)
			{
				guc_packet->UpdateDEPV(LocT[i]->GetLPV()->GetX(), LocT[i]->GetLPV()->GetY()); //update DE location inside header
				break;
			}
		}
		TransmitUnicast(guc_packet);
	}
}
void Vehicle::HandleGUCPacket(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		
		cout << "[GUC] " << GN_ADDR << " receives GUC, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to " << packet->GetDEPV()->GetGN_ADDR() << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);

		if (packet->GetDEPV()->GetGN_ADDR() != GN_ADDR) //forward
		{
			ForwardUnicast(packet);
		}
		else //receive
		{
			GatherReceptionData(packet);
			cout << "[GUC] " << GN_ADDR << " is a destination of GUC from " << packet->GetSOPV()->GetGN_ADDR() << endl;
			for (int i = 0; i < LocT.size(); i++)
			{
				if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
				{
					FlushUCBuffer(i);
					if (LocT[i]->IsPending())
					{
						LocT[i]->SetPending(false);
						FlushLSBuffer(i);
					}
					break;
				}
			}
			FlushBCBuffer();
		}
	}
	else
	{
		cout << "[GUC] " << GN_ADDR << " receives duplicated GUC, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to " << packet->GetDEPV()->GetGN_ADDR() << endl;
	}
}
bool Vehicle::IsLSNeeded(string req_GN_ADDR)
{
	if (LocT.empty())
	{
		return true;
	}
	else
	{
		for (int i = 0; i < LocT.size(); i++)
		{
			if (LocT[i]->GetLPV()->GetGN_ADDR() == req_GN_ADDR) //requested vehicle is in LocT
			{
				if (LocT[i]->GetLPV()->GetX() != NULL) //position of requested vehicle is correct
				{
					return false;
				}
			}
			else if (i == LocT.size() - 1) //requested vehicle is not in LocT
			{
				return true;
			}
		}
	}
}

//-----LS REQUEST------------------------------------------------------------------------------------------------------------------------//

void Vehicle::SendLSRequest(Packet* packet)//packet is GUC packet
{
	cout << "[LS REQUEST] " << GN_ADDR << " requires LOCATION SERVICE to send GUC, SN: " << packet->GetSN() << " to " << packet->GetDEPV()->GetGN_ADDR() << endl;
	bool LS_flag = true;

	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetLPV()->GetGN_ADDR() == packet->GetDEPV()->GetGN_ADDR())
		{
			if (LocT[i]->IsPending()) //LS Request was already sent
			{
				cout << "[LS REQUEST] " << GN_ADDR << " LS Request looking for " << packet->GetDEPV()->GetGN_ADDR() << " was already sent before" << endl;
				LS_flag = false;
				break;
			}
		}
	}
	if (LS_flag)
	{
		LocTEntry* sought_veh = new LocTEntry(new LPV(packet->GetDEPV()->GetGN_ADDR(), NULL, NULL, NULL, NULL, s->GetTime()), s->GetTime(), false);
		sought_veh->SetPending(true);
		LocT.push_back(sought_veh); //add to LocT
		AddToLSBuffer(packet);

		TransmitLSRequest(packet);
	}
}
void Vehicle::TransmitLSRequest(Packet* packet)
{
	Packet* ls_req_packet = new LSReqPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), packet->GetDEPV()->GetGN_ADDR(), s->GetTime());
	GatherTransmissionData(ls_req_packet);
	
	cout << "[LS REQUEST] " << GN_ADDR << "	wants to transmit LS Request, SN: " << ls_req_packet->GetSN() << " looking for " << packet->GetDEPV()->GetGN_ADDR() << endl;
	if (IsChannelReady(ls_req_packet))
	{
		cout << "[LS REQUEST] " << GN_ADDR << "	sends LS Request, SN: " << ls_req_packet->GetSN() << " as broadcast" << endl;
		ls_req_packet->ScheduleTransmission(s->GetTime());
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (e->vehicles[i]->GetGN_ADDR() != GN_ADDR)
			{
				e->vehicles[i]->ReceivePacket(ls_req_packet);
			}
		}
	}
	else //Add to MAC Buffer
	{
		cout << "[LS REQUEST] " << " LS Request, SN: " << ls_req_packet->GetSN() << " added to the MAC buffer" << endl;
		AddToMACBuffer(ls_req_packet, "BCAST");
	}
}
void Vehicle::ReTransmitLSRequest(int index)
{
	LocT[index]->IncrementLSCounter();
	if (LocT[index]->GetLSCounter() < 10)
	{
		LocT[index]->SetLSTimer(s->GetTime() + 10); //1s
		cout << "[LS REQUEST] " << GN_ADDR << "  begins " << LocT[index]->GetLSCounter() << " retransmission of LS REQUEST" << endl;
		Packet* packet = LocT[index]->GetLSBuffer().front();
		TransmitLSRequest(packet);
	}
	else
	{
		LocT[index]->ResetLSCounter();
		LocT[index]->PopLSBuffer();
		LocT[index]->ResetLSTimer();
	}
}
void Vehicle::HandleLSRequest(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		GatherReceptionData(packet);
		cout << "[LS REQUEST] " << GN_ADDR << "	receives LS Request, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " looking for " << packet->GetReq_GN_ADDR() << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);

		if (packet->GetReq_GN_ADDR() != GN_ADDR) //forward
		{
			ForwardBroadcast(packet);
		}
		else //receive and send reply
		{
			cout << "[LS REQUEST] " << GN_ADDR << "	is destination of LS Request" << endl;
			Packet* ls_reply_packet = new LSRepPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), new SPV(packet->GetSOPV()->GetGN_ADDR(), packet->GetSOPV()->GetX(), packet->GetSOPV()->GetY(), packet->GetSOPV()->GetTimestamp()), s->GetTime());
			GatherTransmissionData(ls_reply_packet);
			cout << "[LS REPLY] " << GN_ADDR << " wants to transmit LS Reply to " << packet->GetSOPV()->GetGN_ADDR() << endl;
			TransmitUnicast(ls_reply_packet);
		}
	}
	else
	{
		cout << "[LS REQUEST] " << GN_ADDR << "	receives duplicated LS Request, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " looking for " << packet->GetReq_GN_ADDR() << endl;
	}
}

//-----LS REPLY--------------------------------------------------------------------------------------------------------------------------//

void Vehicle::HandleLSReply(Packet* packet)
{
	if (!IsPacketDuplicate(packet))
	{
		cout << "[LS REPLY] " << GN_ADDR << "	receives LS Reply, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to " << packet->GetDEPV()->GetGN_ADDR() << endl;
		UpdateLocT(packet->GetSOPV(), false);
		AddToDPL(packet);

		if (packet->GetDEPV()->GetGN_ADDR() != GN_ADDR) //forward
		{
			ForwardUnicast(packet);
		}
		else //receive
		{
			GatherReceptionData(packet);
			cout << "[LS REPLY] " << GN_ADDR << "	is destination of LS Reply" << endl;
			for (int i = 0; i < LocT.size(); i++)
			{
				if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
				{
					FlushUCBuffer(i);
					if (LocT[i]->IsPending())
					{
						LocT[i]->SetPending(false);
						FlushLSBuffer(i);
					}
					break;
				}
			}
			FlushBCBuffer();
		}
	}
	else
	{
		cout << "[LS REPLY] " << GN_ADDR << "	receives duplicated LS Reply, SN: " << packet->GetSN() << " from " << packet->GetSOPV()->GetGN_ADDR() << " to " << packet->GetDEPV()->GetGN_ADDR() << endl;
	}
}

//-----BROADCAST TRANSMISSION------------------------------------------------------------------------------------------------------------//

void Vehicle::TransmitBroadcast(Packet* packet)
{
	if (IsChannelReady(packet))
	{
		cout << "[BROADCAST] " << GN_ADDR << " transmits broadcast packet, SN: " << packet->GetSN() << endl;
		packet->ScheduleTransmission(s->GetTime());
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (e->vehicles[i]->GetGN_ADDR() != GN_ADDR)
			{
				e->vehicles[i]->ReceivePacket(packet);
			}
		}
	}
	else //Add to MAC Buffer
	{
		cout << "[BROADCAST] " << GN_ADDR << " adds packet, SN: " << packet->GetSN() << " to the MAC buffer" << endl;
		AddToMACBuffer(packet, "BCAST");
	}
}
void Vehicle::ForwardBroadcast(Packet* packet)
{
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
		{
			FlushUCBuffer(i);
			if (LocT[i]->IsPending())
			{
				LocT[i]->SetPending(false);
				FlushLSBuffer(i);
			}
			break;
		}
	}
	FlushBCBuffer();
	
	packet->GetBasicHeader()->DecrementRHL();
	if (packet->GetBasicHeader()->GetRHL() >= 0)
	{
		Packet* forwarded_packet = CopyPacket(packet);
		TransmitBroadcast(forwarded_packet);
	}
}

//-----UNICAST TRANSMISSION--------------------------------------------------------------------------------------------------------------//

void Vehicle::TransmitUnicast(Packet* packet)
{
	string next_hop_gn_addr = GreedyForwardingAlgorithm(packet->GetDEPV()->GetX(), packet->GetDEPV()->GetY(), packet->GetSOPV()->GetGN_ADDR()); //forwarding algorithm
	if (next_hop_gn_addr != "0")
	{
		if (IsChannelReady(packet))
		{
			packet->ScheduleTransmission(s->GetTime());
			for (int i = 0; i < e->vehicles.size(); i++)
			{
				if (e->vehicles[i]->GetGN_ADDR() == next_hop_gn_addr)
				{
					cout << "[UNICAST] " << "Packet, SN: " << packet->GetSN() << " forwarded from " << GN_ADDR << " to " << e->vehicles[i]->GetGN_ADDR() << endl;
					e->vehicles[i]->ReceivePacket(packet);
					break;
				}
			}
		}
		else //Add to MAC Buffer
		{
			cout << "[UNICAST] " << GN_ADDR << " adds packet, SN: " << packet->GetSN() << " to the MAC buffer" << endl;
			AddToMACBuffer(packet, next_hop_gn_addr);
		}
	}
	else //Add to UC Buffer
	{
		cout << "[UNICAST] " << GN_ADDR << "  did not found a forwarder (packet added to UC buffer)" << endl;
		AddToUCBuffer(packet);
	}
}
void Vehicle::ForwardUnicast(Packet* packet)
{
	for (int i = 0; i < LocT.size(); i++) //update LocT of DE
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetDEPV()->GetGN_ADDR() && LocT[i]->IsNeighbour())
		{
			//update location of DE taking timestamp into account
			if (LocT[i]->GetLPV()->GetTimestamp() < packet->GetDEPV()->GetTimestamp()) //packet timestamp is fresher - update LocT
			{
				UpdateLocT(new LPV(packet->GetDEPV()->GetGN_ADDR(), packet->GetDEPV()->GetX(), packet->GetDEPV()->GetY(), 0, 0, s->GetTime()), false);
			}
			else //LocT timestamp is fresher - update packet
			{
				packet->UpdateDEPV(LocT[i]->GetLPV()->GetX(), LocT[i]->GetLPV()->GetY()); //update position coordinates of DE PV
				packet->GetDEPV()->SetTimestamp(LocT[i]->GetLPV()->GetTimestamp()); //update timestapm of DE PV
			}
			break;
		}
		else if (i == LocT.size() - 1)
		{
			UpdateLocT(new LPV(packet->GetDEPV()->GetGN_ADDR(), packet->GetDEPV()->GetX(), packet->GetDEPV()->GetY(), 0, 0, s->GetTime()), false);
		}
	}
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
		{
			FlushUCBuffer(i);
			if (LocT[i]->IsPending())
			{
				LocT[i]->SetPending(false);
				FlushLSBuffer(i);
			}
			break;
		}
	}
	FlushBCBuffer();

	packet->GetBasicHeader()->DecrementRHL();
	if (packet->GetBasicHeader()->GetRHL() > 0)
	{
		TransmitUnicast(packet);
	}
}

//-----BEACONS---------------------------------------------------------------------------------------------------------------------------//

void Vehicle::SendBeacon()
{
	next_beacon_time = s->GetTime() + e->UniRand(23, 37);
	Packet* beacon = new BeaconPacket(new LPV(GN_ADDR, coord_X, coord_Y, velocity, heading, s->GetTime()), s->GetTime());
	GatherTransmissionData(beacon);
	if (IsChannelReady(beacon))
	{
		cout << "[BEACON] " << GN_ADDR << " sends beacon" << endl;
		beacon->ScheduleTransmission(s->GetTime());
		for (int i = 0; i < e->vehicles.size(); i++)
		{
			if (e->vehicles[i]->GetGN_ADDR() != GN_ADDR)
			{
				e->vehicles[i]->ReceivePacket(beacon);
			}
		}
	}
	else
	{
		AddToMACBuffer(beacon, "BCAST");
	}
}
void Vehicle::HandleBeaconPacket(Packet* bp)
{
	GatherReceptionData(bp);
	cout << "[BEACON] " << GN_ADDR << " receives beacon from " << bp->GetSOPV()->GetGN_ADDR() << endl;
	UpdateLocT(bp->GetSOPV(), true);
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == bp->GetSOPV()->GetGN_ADDR())
		{
			LocT[i]->SetNeighbour(true);
			FlushUCBuffer(i);
			if (LocT[i]->IsPending())
			{
				LocT[i]->SetPending(false);
				FlushLSBuffer(i);
			}
			break;
		}
	}
	FlushBCBuffer();
}

//-----BUFFERS HANDLING------------------------------------------------------------------------------------------------------------------//

void Vehicle::FlushLSBuffer(int index)
{
	for (int i = 0; i < LocT[index]->GetLSBuffer().size(); i++)
	{
		cout << "[LS BUFFER] " << GN_ADDR << " flushes its LS Buffer for " << LocT[index]->GetGN_ADDR() << endl;
		Packet* packet = LocT[index]->GetLSBuffer().front();
		packet->UpdateDEPV(LocT[index]->GetLPV()->GetX(), LocT[index]->GetLPV()->GetY());
		LocT[index]->PopLSBuffer();
		TransmitUnicast(packet);
	}
}
void Vehicle::FlushUCBuffer(int index)
{
	for (int i = 0; i < LocT[index]->GetUCBuffer().size(); i++)
	{
		cout << "[UC BUFFER] " << GN_ADDR << " flushes its UC Buffer for " << LocT[index]->GetGN_ADDR() << " packet nom. " << i << endl;
		Packet* packet = LocT[index]->GetUCBuffer().front();
		packet->UpdateDEPV(LocT[index]->GetLPV()->GetX(), LocT[index]->GetLPV()->GetY());
		LocT[index]->PopUCBuffer();
		TransmitUnicast(packet);
	}
}
void Vehicle::FlushBCBuffer()
{
	for(int i=0; i<BC_buffer.size(); i++)
	{
		cout << "[BC BUFFER] " << GN_ADDR << " flushes its BC Buffer " << endl;
		Packet* packet = BC_buffer.front();
		BC_buffer.pop();

		switch (packet->GetCommonHeader()->GetHT())
		{
			case 3: //GAC
			{
				TransmitGeoAnycast(packet);
				break;
			}
			case 4: //GBC
			{
				TransmitGeoBroadcast(packet);
				break;
			}
			case 5: //TSB
			{
				TransmitBroadcast(packet);
			}
		}	
	}
}
void Vehicle::FlushMACBuffer()
{
	while (MAC_buffer.size() > 0)
	{
		Packet* packet = MAC_buffer.front()->GetPacket();
		string next_hop = MAC_buffer.front()->GetNextHop();
		if (IsChannelReady(packet))
		{
			packet->ScheduleTransmission(s->GetTime());
			MAC_buffer.pop();
			if (next_hop == "BCAST")
			{
				cout << "[MAC BUFFER] " << GN_ADDR << " forwards packet, SN: "<<packet->GetSN()<<" from its MAC Buffer as broadcast" << endl;
				for (int i = 0; i < e->vehicles.size(); i++)
				{
					if (e->vehicles[i]->GetGN_ADDR() != GN_ADDR)
					{
						e->vehicles[i]->ReceivePacket(packet);
					}
				}
			}
			else
			{
				cout << "[MAC BUFFER] " << GN_ADDR << " forwards packet, SN: " << packet->GetSN() << " from its MAC Buffer to " << next_hop << endl;
				for (int i = 0; i < e->vehicles.size(); i++)
				{
					if (e->vehicles[i]->GetGN_ADDR() == next_hop)
					{
						cout << "[UNICAST] " << "Packet, SN: " << packet->GetSN() << " forwarded from " << GN_ADDR << " to " << e->vehicles[i]->GetGN_ADDR() << endl;
						packet->ScheduleTransmission(s->GetTime());
						e->vehicles[i]->ReceivePacket(packet);
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
}
void Vehicle::AddToLSBuffer(Packet* packet)
{
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetDEPV()->GetGN_ADDR())
		{
			LocT[i]->AddToLSBuffer(packet);
			LocT[i]->SetLSTimer(s->GetTime() + 10); //1s
			break;
		}
	}
}
void Vehicle::AddToUCBuffer(Packet* packet)
{
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetDEPV()->GetGN_ADDR())
		{
			LocT[i]->AddToUCBuffer(packet);
			break;
		}
	}
}
void Vehicle::AddToBCBuffer(Packet* packet)
{
	BC_buffer.push(packet);
	if (BC_buffer.size() > 10) //not defined in standard - assumption
	{
		BC_buffer.pop();
	}
}
void Vehicle::AddToMACBuffer(Packet* packet, string address)
{
	MAC_buffer.push(new MACBufferElement(packet, address));
}
void Vehicle::DropFrontBC()
{
	BC_buffer.pop();
}
void Vehicle::DropFrontMAC()
{
	MAC_buffer.pop();
}

//-----FORWARDING ALGORITHMS-------------------------------------------------------------------------------------------------------------//

string Vehicle::GreedyForwardingAlgorithm(double pos_x, double pos_y, string source_GN_ADDR) 
{
	//MFR - most forward within radius - distance between this, and destination
	double MFR = sqrt(pow(pos_x - this->GetX(), 2) + pow(pos_y - this->GetY(), 2));
	double initial_distance = MFR;
	LocTEntry* NH = NULL; //next hop
	string NH_LL_ADDR;

	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->IsNeighbour() && LocT[i]->GetGN_ADDR() != source_GN_ADDR)
		{
			double DIST = sqrt(pow(pos_x - LocT[i]->GetLPV()->GetX(), 2) + pow(pos_y - LocT[i]->GetLPV()->GetY(), 2)); //distance between LocT[i] and destination
			if (DIST < MFR)
			{
				NH = LocT[i];
				MFR = DIST;
			}
		}
	}
	if (MFR < initial_distance)
	{
		NH_LL_ADDR = NH->GetGN_ADDR(); //LL_ADDR = GN_ADDR 
	}
	else
	{
		NH_LL_ADDR = "0";
	}
	return NH_LL_ADDR;
}
string Vehicle::GeoBroadcastLineForwardingAlgorithm(Packet* packet)
{
	double distance = sqrt(pow(packet->GetDestX() - this->GetX(), 2) + pow(packet->GetDestY() - this->GetY(), 2));

	if (distance <= packet->GetRadius()) //GeoAdhoc router is inside or at the border of target area
	{
		return "BCAST";
	}
	else //GeoAdhoc router is outside of target area
	{
		if (packet->GetSenderGN_ADDR() != GN_ADDR) // if this vehicle is not source
		{
			double sender_dist = 0.0;
			bool sender_flag = false; //indicates whether the sender of packet is known in LocT

			for (int i = 0; i < LocT.size(); i++)
			{
				if (LocT[i]->GetGN_ADDR() == packet->GetSenderGN_ADDR())
				{
					sender_dist = sqrt(pow(packet->GetDestX() - LocT[i]->GetLPV()->GetX(), 2) + pow(packet->GetDestY() - LocT[i]->GetLPV()->GetY(), 2));
					sender_flag = true;
					break;
				}
			}

			if (sender_flag)
			{
				if (sender_dist > packet->GetRadius()) //sender is outside of target area
				{
					return GreedyForwardingAlgorithm(packet->GetDestX(), packet->GetDestY(), packet->GetSOPV()->GetGN_ADDR()); //Greedy algorithm returns address or 0 if no forwarder found
				}
				else //sender is inside of target area
				{
					return "-1"; //packet is to be discarded
				}
			}
			else
			{
				return "BCAST";
			}
		}
		else //this vehicle is a source so greedy algorithm is in use
		{
			return GreedyForwardingAlgorithm(packet->GetDestX(), packet->GetDestY(), packet->GetSOPV()->GetGN_ADDR()); //Greedy algorithm returns address or 0 if no forwarder found
		}	
	}
}

//-----DPL - Duplicate Packet List-------------------------------------------------------------------------------------------------------//

bool Vehicle::IsPacketDuplicate(Packet* packet)
{
	bool duplicate_flag = false;
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR()) //SO already in LocT
		{
			for (int k = 0; k < LocT[i]->GetDPL().size(); k++) //check for packet duplicates
			{
				if (LocT[i]->GetDPL()[k] == packet->GetSN())
				{
					duplicate_flag = true;
					break;
				}
			}
			break;
		}
	}
	return duplicate_flag;
}
void Vehicle::AddToDPL(Packet* packet)
{
	for (int i = 0; i < LocT.size(); i++)
	{
		if (LocT[i]->GetGN_ADDR() == packet->GetSOPV()->GetGN_ADDR())
		{
			LocT[i]->AddToDPL(packet->GetSN());
			if (LocT[i]->GetDPL().size() > 8) //max number of elements in DPL is 8
			{
				LocT[i]->DropInDPL(); //remove first one 
			}
			break;
		}
	}
}

//-----LocT UPDATERS--------------------------------------------------------------------------------------------------------------------//

void Vehicle::UpdateLocT(LPV* so_pv, bool is_neighbour)
{
	if (!LocT.empty())
	{
		for (int i = 0; i < LocT.size(); i++)
		{
			if (so_pv->GetGN_ADDR() == LocT[i]->GetLPV()->GetGN_ADDR())
			{
				//update LocT without changing IS_NEIGHBOUR and IS_PENDING
				LocT[i]->GetLPV()->SetX(so_pv->GetX());
				LocT[i]->GetLPV()->SetY(so_pv->GetY());
				LocT[i]->GetLPV()->SetV(so_pv->GetV());
				LocT[i]->GetLPV()->SetH(so_pv->GetH());
				LocT[i]->GetLPV()->SetTimestamp(so_pv->GetTimestamp());
				LocT[i]->SetLT(s->GetTime()); //20s added inside LocTEntry.cpp
				break;
			}
			else if (i == LocT.size() - 1)
			{
				LocT.push_back(new LocTEntry(so_pv, s->GetTime(), is_neighbour)); //20s added inside LocTEntry.cpp
			}
		}
	}
	else
	{
		LocT.push_back(new LocTEntry(so_pv, s->GetTime(), is_neighbour)); //20s added inside LocTEntry.cpp
	}
}
void Vehicle::DropLocTEntry(int index)
{
	LocT.erase(LocT.begin() + index);
}

//-----SETTERS AND GETTERS---------------------------------------------------------------------------------------------------------------//

std::string Vehicle::GetGN_ADDR()
{
	return GN_ADDR;
}
double Vehicle::GetX()
{
	return coord_X;
}
double Vehicle::GetY()
{
	return coord_Y;
}
double Vehicle::GetV()
{
	return velocity;
}
double Vehicle::GetH()
{
	return heading;
}
int Vehicle::GetBeaconTime()
{
	return next_beacon_time;
}
int Vehicle::GetTransmissionTime()
{
	return next_trans_time;
}
int Vehicle::GetResourceAllocationTime()
{
	return next_allocation_time;
}
vector<LocTEntry*> Vehicle::GetLocT()
{
	return LocT;
}
queue<Packet*> Vehicle::GetBC_buffer()
{
	return BC_buffer;
}
queue<MACBufferElement*> Vehicle::GetMAC_buffer()
{
	return MAC_buffer;
}
queue<Packet*> Vehicle::GetReceptionQueue()
{
	return reception_queue;
}
vector<vector<ResourcesMatrixElement*>> Vehicle::GetResourcesMatrix()
{
	return resources_matrix;
}
void Vehicle::SetX(double x)
{
	coord_X = x;
}
void Vehicle::SetY(double y)
{
	coord_Y = y;
}
void Vehicle::SetV(double v)
{
	velocity = v;
}
void Vehicle::SetH(double h)
{
	heading = h;
}
bool Vehicle::IsActive()
{
	return active;
}
void Vehicle::Activate()
{
	active = true;
}
void Vehicle::Deactivate()
{
	active = false;
}

//---------------------------------------------------------------------------------------------------------------------------------------//