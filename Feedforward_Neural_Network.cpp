#include "StdAfx.h"
#include "Feedforward_Neural_Network.h"


Feedforward_Neural_Network::Feedforward_Neural_Network(int num_input, int num_hidden, int num_output)
{
	this->num_input	 = num_input;
	this->num_hidden = num_hidden;
	this->num_output = num_output;

	output_weight = new double*[num_output];
	for(int i = 0;i < num_output;i++){
		output_weight[i] = new double[num_hidden + 1];
	}
	hidden_weight = new double*[num_hidden];
	for(int i = 0;i < num_hidden;i++){
		hidden_weight[i] = new double[num_input + 1];
	}
}
Feedforward_Neural_Network::~Feedforward_Neural_Network()
{
	for(int i = 0;i < num_output;i++){
		delete[] output_weight[i];
	}
	for(int i = 0;i < num_hidden;i++){
		delete[] hidden_weight[i];
	}
	delete[] hidden_weight;
	delete[] output_weight;
}

bool Feedforward_Neural_Network::saveWeight(char* filename)
{
	//open file for reading
	fstream outputFile;
	outputFile.open(filename, ios::out);

	if ( outputFile.is_open() )
	{
		outputFile.precision(50);		

		//output weights
		for ( int i=0; i < num_output; i++ ) 
		{
			for ( int j=0; j <= num_hidden; j++ ) 
			{
				outputFile << output_weight[i][j] << ",";				
			}
		}

		for ( int i=0; i < num_hidden; i++ ) 
		{		
			for ( int j=0; j <= num_input; j++ ) 
			{
				if(i==num_hidden-1 && j==num_input) outputFile << hidden_weight[i][j]; 
				else outputFile << hidden_weight[i][j] << ",";					
			}
		}

		//print success
		cout << endl << "Neuron weights saved to '" << filename << "'" << endl;

		//close file
		outputFile.close();
		return true;
	}
	else 
	{
		cout << endl << "Error - Weight output file '" << filename << "' could not be created: " << endl;			
		return false;
	}

}


bool Feedforward_Neural_Network::loadWeights(char* filename)
{
	//open file for reading
	fstream inputFile;
	inputFile.open(filename, ios::in);

	if ( inputFile.is_open() )
	{
		vector<double> weights;
		string line = "";

		//read data
		while ( !inputFile.eof() )
		{
			getline(inputFile, line);				

			//process line
			if (line.length() > 2 ) 
			{				
				//store inputs		
				char* cstr = new char[line.size()+1];
				char* t;
				strcpy(cstr, line.c_str());

				//tokenise
				int i = 0;
				t=strtok (cstr,",");

				while ( t!=NULL )
				{	
					weights.push_back( atof(t) );

					//move token onwards
					t = strtok(NULL,",");
					i++;			
				}

				//free memory
				delete[] cstr;
			}
		}	

		//check if sufficient weights were loaded
		if ( weights.size() != ( num_output * (num_hidden+1) + num_hidden * (num_input+1 )) ) 
		{
			cout << endl << "Error - Incorrect number of weights in input file: " << filename << endl;

			//close file
			inputFile.close();

			return false;
		}
		else
		{
			//set weights
			int pos = 0;

			for ( int i=0; i < num_output; i++ ) 
			{
				for ( int j=0; j <= num_hidden; j++ ) 
				{
					output_weight[i][j] = weights[pos++];					
				}
			}

			for ( int i=0; i < num_hidden; i++ ) 
			{		
				for ( int j=0; j <= num_input; j++ ) 
				{
					hidden_weight[i][j] = weights[pos++];						
				}
			}	

			//print success
			cout << endl << "Neuron weights loaded successfuly from '" << filename << "'" << endl;

			//close file
			inputFile.close();
			

			return true;
		}		
	}
	else 
	{
		cout << endl << "Error - Weight input file '" << filename << "' could not be opened: " << endl;
		return false;
	}

}

void Feedforward_Neural_Network::Test(double input[], double output[])
{
	double *hidden = new double[num_hidden];

	Compute_Output(input, hidden, output);

	delete[] hidden;
}
void Feedforward_Neural_Network::Train(int max_epoch, int num_train, double learning_rate, double **input, double **target_output)
{
	int num_epoch = 0;
	//int max_epoch = 10000;

	double *hidden				= new double[num_hidden];
	double *hidden_derivative	= new double[num_hidden];
	double *output				= new double[num_output];
	double *output_derivative	= new double[num_output];

	srand(0);
	for(int i = 0;i < num_hidden;i++){
		for(int j = 0;j < num_input + 1;j++){
			hidden_weight[i][j] = 0.2 * rand() / RAND_MAX - 0.1;
		}
	}
	for(int i = 0;i < num_output;i++){
		for(int j = 0;j < num_hidden + 1;j++){
			output_weight[i][j] = 0.2 * rand() / RAND_MAX - 0.1;
		}
	}

	do{
		double error = 0;

		for(int i = 0;i < num_train;i++){
			Compute_Output(input[i], hidden, output);

			// 출력미분값 계산
			for(int j = 0;j < num_output;j++){
				output_derivative[j] = learning_rate * (output[j] - target_output[i][j]) * (1 - output[j]) * output[j];
			}

			// 출력가중치 조정
			for(int j = 0;j < num_output;j++){
				for(int k = 0;k < num_hidden;k++){
					output_weight[j][k] -= output_derivative[j] * hidden[k];
				}
				output_weight[j][num_hidden] -= output_derivative[j];
			}

			// 은닉미분값 계산
			for(int j = 0;j < num_hidden;j++){
				double sum = 0;

				for(int k = 0;k < num_output;k++){
					sum += output_derivative[k] * output_weight[k][j];
				}
				hidden_derivative[j] = sum * (1 - hidden[j]) * hidden[j];
			}

			// 은닉가중치 조정
			for(int j = 0;j < num_hidden;j++){
				for(int k = 0;k < num_input;k++){
					hidden_weight[j][k] -= hidden_derivative[j] * input[i][k];
				}
				hidden_weight[j][num_input] -= hidden_derivative[j];
			}

			// 오차 계산
			for(int j = 0;j < num_output;j++){
				error += 0.5 * (output[j] - target_output[i][j]) * (output[j] - target_output[i][j]);
			}
		}
		if(num_epoch % 500 == 0){
			printf("반복횟수: %d, 오차: %lf\n", num_epoch, error);
		}
	}while(num_epoch++ < max_epoch);

 	delete[] hidden;
 	delete[] output;

}

void Feedforward_Neural_Network::processLine222( string &line , int nInputs, int nTargets, double *input, double *output)
{
	//create new pattern and target
	double* pattern = new double[nInputs];
	double* target  = new double[nTargets];

	//store inputs		
	char* cstr = new char[line.size()+1];
	char* t;
	strcpy(cstr, line.c_str());



	//tokenise
	int i = 0;
	t=strtok (cstr,",");

	while ( t!=NULL && i < (nInputs + nTargets) )
	{	
		if ( i < nInputs ) pattern[i] = atof(t);
		else			   target[i - nInputs] = atof(t);

		//move token onwards
		t = strtok(NULL,",");
		i++;			
	}
	for(i=0; i<nInputs ; i++)input[i]  = pattern[i];
	for(i=0; i<nTargets; i++)output[i] = target[i];


}


void Feedforward_Neural_Network::Compute_Output(double input[], double hidden[], double output[])
{
	for(int i = 0;i < num_hidden;i++){
		double sum = 0;

		for(int j = 0;j < num_input;j++){
			sum += input[j] * hidden_weight[i][j];
		}
		sum += hidden_weight[i][num_input];
		hidden[i] = 1 / (1 + exp(-sum));
	}
	for(int i = 0;i < num_output;i++){
		double sum = 0;

		for(int j = 0;j < num_hidden;j++){
			sum += hidden[j] * output_weight[i][j];
		}
		sum += output_weight[i][num_hidden];
		output[i] = 1 / (1 + exp(-sum));
	}
}


