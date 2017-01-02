#pragma once

#include <iostream>
#include <string>
//#include <math.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <vector>
#include <fstream>
using namespace std;
class Feedforward_Neural_Network
{
	
public:
	Feedforward_Neural_Network(int num_input, int num_hidden, int num_output);
	~Feedforward_Neural_Network();
private:
	int num_input;
	int num_hidden;
	int num_output;

	double **hidden_weight;
	double **output_weight;

	// ¾Æ¿ôÇ² °è»ê
	void Compute_Output(double input[], double hidden[], double output[]);

public:
	bool saveWeight(char* filename);


	bool loadWeights(char* filename);

	void Test(double input[], double output[]);
	void Train(int max_epoch, int num_train, double learning_rate, double **input, double **target_output);
	void processLine222( string &line , int nInputs, int nTargets, double *input, double *output);

};

