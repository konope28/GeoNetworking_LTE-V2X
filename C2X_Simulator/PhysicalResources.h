#ifndef SIMULATION_PHYSICALRESOURCES_H_
#define SIMULATION_PHYSICALRESOURCES_H_
#include <vector>
using namespace std;

class ShortSensingWindow //5 subchannels x 100 subframes(100ms)
{
public:
	ShortSensingWindow();
	~ShortSensingWindow();
	double GetPathLoss(int subframe, int subchannel);
	void SetPathLoss(int subframe, int subchannel, double loss);

private:
	vector<vector<double>> short_sensing_window;
};

class SensingVectorElement 
{
public:
	SensingVectorElement(int i_frame, int i_channel, double loss);
	~SensingVectorElement();
	int GetSubframeIndex();
	int GetSubchannelIndex();
	double GetPathLoss();
	void SetPathLoss(double loss);

private:
	int subframe_index;
	int subchannel_index;
	double path_loss;
};

class ResourcesMatrixElement 
{
public:
	ResourcesMatrixElement();
	~ResourcesMatrixElement();
	bool IsUsed();
	void SetUsed(bool used);

private:
	bool used;
};
#endif /*SIMULATION_PHYSICALRESOURCES_H_*/