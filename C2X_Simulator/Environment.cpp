#include "environment.h"

Environment::Environment() 
{
	lambda_transmission = 1.0 / 100.0;
	lambda_hops = 1.0 / 4.0;
	st_dev = 5.5;
}

Environment::~Environment() {}

double Environment::Rand()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);

	double res = dis(gen);
	return dis(gen);

}
int Environment::UniRand(int start, int end)
{
	return Rand()*(end - start) + start;
}
int Environment::ExpoRandTransmission()
{
	int result = 0;
	while (result == 0)
	{
		double k = this->Rand();	//uniform variable
		result = -(1.0 / lambda_transmission)*log(k);
	}
	return result;
}
int Environment::ExpoRandMultiHop()
{
	int result = 0;
	while (result < 2 || result > 10)
	{
		double k = this->Rand();	//uniform variable
		result = -(1.0 / lambda_hops)*log(k);
	}
	int a = result;
	return result;
}
double Environment::NormalRand()
{
	double random_num = 0;
	double k[12] = {};
	for (int i = 0; i < 12; i++)
	{
		k[i] = this->Rand();	//12 independent uniform variables
		random_num += k[i];
	}

	return (random_num - 6)*st_dev;	//scaling, mean=0
}