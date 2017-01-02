#pragma once

#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>
using namespace std;
class Rprop
{

public:
	Rprop(int num_input, int num_hidden, int num_output, int num_train);
	~Rprop();
private:
	 int numInput;
	 int numHidden;
	 int numOutput;

	 int num_input;
	 int num_output;
	 int num_train;
	 
	 double learning_convergence_error_rate;

	 //double **test_input;	
	 double **input;		
	 double **target_output;

	 double *inputs;
	 double **ihWeights; // input-hidden
	 double *hBiases;
	 double *hOutputs;

	 double **hoWeights; // hidden-output
	 double *oBiases;
	 double *outputs;

	 //Random rnd;
	
	 double HyperTan(double x);
	 double *Softmax(double *oSums);
	 void ZeroOut(int numRows, int numCols, double **omatrix);
	 void ZeroOut_Array(double *array, int array_length) ;
	 void MakeMatrix(double **results, int rows,int cols, double v) ;// helper for ctor, 
	 void MakeVector(double *results, int len, double v);
	 void InitializeWeights() ;// helper for ctor
	 int Sign(double value);
	 double Accuracy(int numTest, double **testData, double *weights);
	 double MeanSquaredError(int numTrain, double **trainData,  double **patternData,  double *weights);	 
	 int MaxIndex(int numVector, double *vector); 
	 void processLine( string &line , int nInputs, int nTargets, double *input, double *output);
public:	 		
	 void TrainRPROP(int numTrain, double **trainData, double **patternData, int maxEpochs, double *out_wts) ;// using RPROP	 
	 void SetWeights(double *weights);
	 void GetWeights(double *results);
	 void ComputeOutputs(double *xValues, double* retResult);	 	
	 bool loadWeights(char* filename);
	 bool saveWeight(char* filename);
	 void inputfile(char *filename, int num_train);
	 void inputfile_phase(char *filename, int num_train);
	 void inputfile_phase_mag(char *filename, int num_train);
	 void inputMap(std::map<int, double*> waveform_character, int num_train, int num_character, int num_target);
	 void Train(int *pole_file_count, int num_pole);
	 void Train();
	 void Train_RT();
	 void Train_RT(int *pole_file_count, int num_pole);
	 double getErrorRate();
};

