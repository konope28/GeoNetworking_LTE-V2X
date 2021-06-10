#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "simulator.h"
using namespace std;

class Vehicle;

int main()
{
	vector<int> f_step;
	vector<string> f_ID;
	vector<double> f_X;
	vector<double> f_Y;
	vector<double> f_V;
	vector<double> f_H;

	int stop_t;

	//open file with data
	fstream file;
	file.open("veh_data_file.csv", ios::in);
	if (!file.is_open()) throw std::runtime_error("Could not open file");

	vector<string> row;
	string line, word, temp;

	//get simulation time
	getline(file, line);
	stop_t = stoi(line);

	//get data and store into vectors
	while (getline(file, line))
	{
		row.clear();
		stringstream s(line);
		while (getline(s, word, ','))
		{
			row.push_back(word);
		}
		f_step.push_back(stoi(row[0]));
		f_ID.push_back(row[1]);
		f_X.push_back(stod(row[2]));
		f_Y.push_back(stod(row[3]));
		f_V.push_back(stod(row[4]));
		f_H.push_back(stod(row[5]));
	}
	file.close();

	//start simulation
	Simulator* simulator = new Simulator(stop_t, f_step, f_ID, f_X, f_Y, f_V, f_H);
	simulator->Start();

	system("PAUSE");
}