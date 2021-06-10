#ifndef SIMULATION_ENVIRONMENT_H_
#define SIMULATION_ENVIRONMENT_H_
#include "vehicle.h"
#include "packet.h"
#include <vector>
#include <random>
using namespace std;
class Vehicle;

struct Environment
{
	Environment();
	~Environment();
	double Rand();
	int UniRand(int start, int end);
	int ExpoRandTransmission();
	int ExpoRandMultiHop();
	double NormalRand();
	vector<Vehicle*> vehicles;

	double lambda_transmission;
	double lambda_hops;
	double st_dev;
};
#endif /*SIMULATION_ENVIRONMENT_H_*/
