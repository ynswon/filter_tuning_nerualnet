#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
class Rprop_Network
{
	
public:
	Rprop_Network(int num_input, int num_hidden, int num_output);
	~Rprop_Network();
private:
	int num_input;
	int num_hidden;
	int num_output;

	double **hidden_weight;
	double **output_weight;

	// ¾Æ¿ôÇ² °è»ê
	int Sign(double value);
	void Compute_Output(double input[], double hidden[], double output[]);
	void MakeMatrix(double **result, int rows, int cols, double v); // helper for ctor, Train

public:
	bool saveWeight(char* filename);


	bool loadWeights(char* filename);

	void Test(double input[], double output[]);
	void Train(int max_epoch, int num_train, double learning_rate, double **input, double **target_output);
	void processLine222( string &line , int nInputs, int nTargets, double *input, double *output);

};

