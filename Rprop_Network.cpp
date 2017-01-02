#include "StdAfx.h"
#include "Rprop_Network.h"

Rprop_Network::Rprop_Network(int num_input, int num_hidden, int num_output)
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
Rprop_Network::~Rprop_Network()
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

bool Rprop_Network::saveWeight(char* filename)
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


bool Rprop_Network::loadWeights(char* filename)
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

int Rprop_Network::Sign(double value)
{
	if(value > 0 )     return 1;
	else if(value==0) return 0;
	else			  return -1;

}

void Rprop_Network::Test(double input[], double output[])
{
	double *hidden = new double[num_hidden];

	Compute_Output(input, hidden, output);

	delete[] hidden;
}


void Rprop_Network::MakeMatrix(double **result, int rows, int cols, double v) // helper for ctor, Train
{			
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result[i][j] = v;   
}


void Rprop_Network::Train(int max_epoch, int num_train, double learning_rate, double **input, double **target_output)
{
	int num_epoch = 0;
	//int max_epoch = 10000;

	////////////////////////////////Rprop///////////////////////////////////////////

	double* hGradTerms = new double[num_hidden+1]; // intermediate val for h-o weight gradients
    double* oGradTerms = new double[num_output+1]; // output gradients

    
    double* oBiasGradsAcc = new double[num_output+1];
    double* hBiasGradsAcc = new double[num_hidden+1];    
    double* oPrevBiasGradsAcc = new double[num_output+1];
    double* hPrevBiasGradsAcc = new double[num_hidden+1];

    // must save previous weight deltas
    double** hoPrevWeightDeltas = new double*[num_hidden+1];
	double** ihPrevWeightDeltas = new double*[num_input+1];;
	

	double** hoWeightGradsAcc	  = new double*[num_hidden+1];	
    double** ihWeightGradsAcc	  = new double*[num_input+1];;;
	double** hoPrevWeightGradsAcc = new double*[num_hidden+1];;;	
    double** ihPrevWeightGradsAcc = new double*[num_input+1];;;

	double* oPrevBiasDeltas		  = new double[num_output+1];		
    double* hPrevBiasDeltas		  = new double[num_hidden+1];
	

	for(int i = 0;i <= num_hidden;i++){
		hoPrevWeightDeltas[i]	= new double[num_output+1];
		hoWeightGradsAcc[i]		= new double[num_output+1];
		hoPrevWeightGradsAcc[i]	= new double[num_output+1];
		
	}
	
	for(int i = 0;i <= num_input;i++){
		ihPrevWeightDeltas[i]		= new double[num_hidden+1];
		ihWeightGradsAcc[i]			= new double[num_hidden+1];
		ihPrevWeightGradsAcc[i]		= new double[num_hidden+1];
		
	}

	MakeMatrix(hoWeightGradsAcc		, num_hidden+1	, num_output, 0.0); // accumulated over all training data
	MakeMatrix(ihWeightGradsAcc		, num_input+1		, num_hidden, 0.0);
	MakeMatrix(hoPrevWeightGradsAcc , num_hidden+1	, num_output, 0.0); // accumulated, previous iteration
	MakeMatrix(ihPrevWeightGradsAcc , num_input+1		, num_hidden, 0.0);
	MakeMatrix(hoPrevWeightDeltas	, num_hidden+1	, num_output, 0.01);
	MakeMatrix(ihPrevWeightDeltas	, num_input+1		, num_hidden, 0.01);

	for(int i = 0;i < num_output;i++){		
		oPrevBiasDeltas[i] = 0.01;
	}
	for(int i = 0;i < num_hidden;i++){		
		hPrevBiasDeltas[i] = 0.01;
	}	

	double etaPlus = 1.2; // values are from the paper
    double etaMinus = 0.5;
    double deltaMax = 50.0;
    double deltaMin = 1.0E-6;
	///////////////////////////////////////////////////////////////////////////

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


		for(int i=0; i<num_hidden; i++)
			for(int j=0;j<num_output; j++)
				hoWeightGradsAcc [i][j]=0;

		for(int i=0; i<num_input; i++)
			for(int j=0;j<num_hidden; j++)
				ihWeightGradsAcc [i][j]=0;

		for(int i=0; i<num_output; i++) oBiasGradsAcc[i]=0;
		for(int i=0; i<num_hidden; i++) hBiasGradsAcc[i]=0;


		for(int i = 0;i < num_train;i++){
			Compute_Output(input[i], hidden, output);

			// 출력미분값 계산
			for(int j = 0;j < num_output;j++){
				//output_derivative[j] = learning_rate * (output[j] - target_output[i][j]) * (1 - output[j]) * output[j];
				output_derivative[j] =  (output[j] - target_output[i][j]) * (1 - output[j]) * output[j];
			}

			// 출력가중치 조정
			/*for(int j = 0;j < num_output;j++){
				for(int k = 0;k < num_hidden;k++){
					output_weight[j][k] -= output_derivative[j] * hidden[k];
				}
				output_weight[j][num_hidden] -= output_derivative[j];
			}*/

			// 은닉미분값 계산
			for(int j = 0;j < num_hidden;j++){
				double sum = 0;

				for(int k = 0;k < num_output;k++){
					sum += output_derivative[k] * output_weight[k][j];
				}
				hidden_derivative[j] = sum * (1 - hidden[j]) * hidden[j];
			}

			// 은닉가중치 조정
			/*for(int j = 0;j < num_hidden;j++){
				for(int k = 0;k < num_input;k++){
					hidden_weight[j][k] -= hidden_derivative[j] * input[i][k];
				}
				hidden_weight[j][num_input] -= hidden_derivative[j];
			}*/

			
            // add input to h-o component to make h-o weight gradients, and accumulate
            for (int i = 0; i < num_hidden; i++)
            {
                for (int j = 0; j < num_output; j++)
                {
                    double grad = output_derivative[j] * hidden[i];
                    hoWeightGradsAcc[i][j] += grad;
                }
            }

            // the (hidden-to-) output bias gradients
            for (int i = 0; i < num_output; i++)
            {
                double grad = output_derivative[i] * 1.0; // dummy input
                oBiasGradsAcc[i] += grad;
            }

            // add input term to i-h component to make i-h weight gradients and accumulate
            for (int k = 0; k <= num_input; k++)
            {
                for (int j = 0; j < num_hidden; j++)
                {
					double grad = 0; 
					if(k==num_input) 
						 grad = hidden_derivative[j] * 1.0;
					else
						 grad = hidden_derivative[j] * input[i][k];
                    ihWeightGradsAcc[k][j] += grad;
                }
            }

            // the (input-to-) hidden bias gradient
            for (int i = 0; i < num_hidden; i++)
            {
                double grad = hidden_derivative[i] * 1.0;
                hBiasGradsAcc[i] += grad;
            }


			// 오차 계산
			for(int j = 0;j < num_output;j++){
				error += 0.5 * (output[j] - target_output[i][j]) * (output[j] - target_output[i][j]);
			}
			
		}
		
		
		if(num_epoch % 500 == 0){
			printf("반복횟수: %d, 오차: %lf\n", num_epoch, error/num_train);
		}

		double delta = 0.0;

        for (int i = 0; i <= num_input; ++i)
        {
            for (int j = 0; j < num_hidden; ++j)
            {
                if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] > 0) // no sign change, increase delta
                {
                    delta = ihPrevWeightDeltas[i][j] * etaPlus; // compute delta
                    if (delta > deltaMax) delta = deltaMax; // keep it in range
                    double tmp = -Sign(ihWeightGradsAcc[i][j]) * delta; // determine direction and magnitude
                    hidden_weight[j][i] += tmp; // update weights
                }
                else if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
                {
                    delta = ihPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved for later)
                    if (delta < deltaMin) delta = deltaMin; // keep it in range

                    hidden_weight[j][i] -= ihPrevWeightDeltas[i][j]; // revert to previous weight
                    ihWeightGradsAcc[i][j] = 0; // forces next if-then branch, next iteration					
                }
                else // this happens next iteration after 2nd branch above (just had a change in gradient)
                {
                    delta = ihPrevWeightDeltas[i][j]; // no change to delta
                    // no way should delta be 0 . . . 
                    double tmp = -Sign(ihWeightGradsAcc[i][j]) * delta; // determine direction
                    hidden_weight[j][i] += tmp; // update weights
                }
                //Console.WriteLine(ihPrevWeightGradsAcc[i][j] + " " + ihWeightGradsAcc[i][j]); Console.ReadLine();

                ihPrevWeightDeltas[i][j] = delta; // save delta
                ihPrevWeightGradsAcc[i][j] = ihWeightGradsAcc[i][j]; // save the (accumulated) gradient
            } // j
        } // i

     

        // update hidden-to-output weights
        for (int i = 0; i <= num_hidden; ++i)
        {
            for (int j = 0; j < num_output; ++j)
            {
                if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] > 0) // no sign change, increase delta
                {
					delta = hoPrevWeightDeltas[i][j] * etaPlus; // compute delta
					if (delta > deltaMax) delta = deltaMax;
					double tmp = -Sign(hoWeightGradsAcc[i][j]) * delta; // determine direction
					output_weight[j][i] += tmp; // update
                }
                else if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
                {
                    delta = hoPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved later)
                    if (delta < deltaMin) delta = deltaMin;
                    output_weight[j][i] -= hoPrevWeightDeltas[i][j]; // revert to previous weight
                    hoWeightGradsAcc[i][j] = 0; // forces next branch, next iteration
					
                }
                else // this happens next iteration after 2nd branch above (just had a change in gradient)
                {
                    delta = hoPrevWeightDeltas[i][j]; // no change to delta
                    // no way should delta be 0 . . . 
                    double tmp = -Sign(hoWeightGradsAcc[i][j]) * delta; // determine direction
                    output_weight[j][i] += tmp; // update
                }
                hoPrevWeightDeltas[i][j] = delta; // save delta
                hoPrevWeightGradsAcc[i][j] = hoWeightGradsAcc[i][j]; // save the (accumulated) gradients
            } // j
        } // i



	}while(num_epoch++ < max_epoch);




	for(int i = 0;i <= num_hidden;i++){
		delete[] hoWeightGradsAcc [i];
	}
	for(int i = 0;i <= num_input;i++){
		delete[] ihWeightGradsAcc [i];
	}
	delete[] hoWeightGradsAcc;
	delete[] ihWeightGradsAcc ;


	for(int i = 0;i <= num_hidden;i++){
		delete[] hoPrevWeightGradsAcc [i];
	}
	for(int i = 0;i <= num_input;i++){
		delete[] ihPrevWeightGradsAcc [i];
	}
	delete[] hoPrevWeightGradsAcc ;
	delete[] ihPrevWeightGradsAcc ;


	for(int i = 0;i <= num_hidden;i++){
		delete[] hoPrevWeightDeltas  [i];
	}
	for(int i = 0;i <= num_input;i++){
		delete[] ihPrevWeightDeltas  [i];
	}
	delete[] hoPrevWeightDeltas  ;
	delete[] ihPrevWeightDeltas  ;


	delete[] hGradTerms ;
	delete[] oGradTerms ;
	delete[] oBiasGradsAcc ;
	delete[] hBiasGradsAcc ;
	delete[] oPrevBiasGradsAcc ;
	delete[] hPrevBiasGradsAcc ;
	delete[] oPrevBiasDeltas;
	delete[] hPrevBiasDeltas;

 	delete[] hidden;
 	delete[] output;

}



		

void Rprop_Network::processLine222( string &line , int nInputs, int nTargets, double *input, double *output)
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


void Rprop_Network::Compute_Output(double input[], double hidden[], double output[])
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


