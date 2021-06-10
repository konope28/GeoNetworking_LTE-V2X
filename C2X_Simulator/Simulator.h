#ifndef SIMULATION_SIMULATOR_H_
#define SIMULATION_SIMULATOR_H_
#include "environment.h"
#include "vehicle.h"
#include "beaconpacket.h"
#include "packet.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
class Vehicle;
struct Environment;

class Simulator
{
public:
	Simulator(int stop_t, 
		vector<int> &file_step,
		vector<string> &file_ID,
		vector<double> &file_X,
		vector<double> &file_Y,
		vector<double> &file_V,
		vector<double> &file_H);

	~Simulator();

	void WriteTransmissionFile(int sn, int t_time, string type);
	void WriteReceptionFile(int sn, int t_time, int d_time, int num_nodes);
	int GetTime();
	void UpdateVehPosition();
	void Start();

private:
	Environment* e;
	const int kStopTime;
	int current_time; //1 step = 100ms

	int pos_iter;

	vector<int> file_step;
	vector<string> file_ID;
	vector<double> file_X;
	vector<double> file_Y;
	vector<double> file_V;
	vector<double> file_H;

	ofstream reception_file;
	ofstream transmission_file;
};

#endif /*SIMULATION_SIMULATOR_H_*/