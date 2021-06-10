#include "physicalresources.h"

ShortSensingWindow::ShortSensingWindow()
{
	for (int k = 0; k < 100; k++)
	{
		vector<double> temp_vec;
		for (int i = 0; i < 5; i++)
		{
			temp_vec.push_back(1000.0);
		}

		short_sensing_window.push_back(temp_vec);
	}
}
ShortSensingWindow::~ShortSensingWindow(){}

double ShortSensingWindow::GetPathLoss(int subframe, int subchannel)
{
	return short_sensing_window[subframe][subchannel];
}
void ShortSensingWindow::SetPathLoss(int subframe, int subchannel, double loss)
{
	short_sensing_window[subframe][subchannel] = loss;
}


SensingVectorElement::SensingVectorElement(int i_frame, int i_channel, double loss)
{
	subframe_index = i_frame;
	subchannel_index = i_channel;
	path_loss = loss;
}
SensingVectorElement::~SensingVectorElement(){}

int SensingVectorElement::GetSubframeIndex()
{
	return subframe_index;
}
int SensingVectorElement::GetSubchannelIndex()
{
	return subchannel_index;
}
double SensingVectorElement::GetPathLoss()
{
	return path_loss;
}
void SensingVectorElement::SetPathLoss(double loss)
{
	path_loss = loss;
}


ResourcesMatrixElement::ResourcesMatrixElement() 
{
	used = false;
}
ResourcesMatrixElement::~ResourcesMatrixElement() {}

bool ResourcesMatrixElement::IsUsed()
{
	return used;
}
void ResourcesMatrixElement::SetUsed(bool u)
{
	used = u;
}