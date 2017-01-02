#include "StdAfx.h"
#include "Rprop.h"
#include <map>
#include "MediaHandler.h"

Rprop::Rprop(int numInput, int numHidden, int numOutput, int num_train)
{
			
    this->numInput = numInput;
    this->numHidden = numHidden;
    this->numOutput = numOutput;
	this->num_train = num_train;

    this->inputs = new double[numInput];
	
//	this->test_input	  	= new double*[num_train];
	this->input				= new double*[num_train];
	this->target_output		= new double*[num_train];
	for(int i = 0;i < num_train;i++){
	//	test_input[i]		= new double[numInput];
		input[i]			= new double[numInput];
		target_output[i]	= new double[numOutput];
	}



	this->ihWeights			= new double*[numInput];
	for(int i = 0;i < numInput;i++){
		ihWeights[i]		= new double[numHidden];		
	}

	this->hoWeights			= new double*[numHidden];
	for(int i = 0;i < numHidden;i++){
		hoWeights[i]		= new double[numOutput];		
	}

	MakeMatrix(this->hoWeights,numHidden, numOutput, 0.0);
	MakeMatrix(this->ihWeights ,numInput, numHidden, 0.0);
    
		
    this->hBiases = new double[numHidden];
    this->hOutputs = new double[numHidden];
    
    this->oBiases = new double[numOutput];
    this->outputs = new double[numOutput];

	srand((unsigned int)time(NULL)); 
} // ctor

Rprop::~Rprop()
{
	 
	
	delete[] inputs;
	 
	for(int i = 0;i < num_train;i++){
		//delete[] test_input[i];
		delete[] input[i];
		delete[] target_output[i];
	}
	//delete[] test_input;
	delete[] input;
	delete[] target_output;
	
	for(int i = 0;i < numInput;i++)
	{
		delete ihWeights[i];
	}
	delete[] ihWeights;

	for(int i = 0;i < numHidden;i++)
	{
		delete[] hoWeights[i];
	}
	delete[] hoWeights;

	delete[] hBiases;
	delete[] hOutputs;

	delete[] oBiases;
	delete[] outputs;

}

double Rprop::getErrorRate()
{
	return learning_convergence_error_rate;
}

void Rprop::MakeMatrix(double **result, int rows,
    int cols, double v) // helper for ctor, Train
{ 
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = v; 
}

int Rprop::Sign(double value)
{
	if(value >0 )     return 1;
	else if(value==0) return 0;
	else			  return -1;

}

void Rprop::MakeVector(double *result, int len, double v) // helper for Train
{
    for (int i = 0; i < len; i++)
        result[i] = v;
}

void Rprop::InitializeWeights() 
{
    // initialize weights and biases to random values between 0.0001 and 0.001
	srand((unsigned int)time(NULL));
    int numWeights = (this->numInput * this->numHidden) + (this->numHidden * this->numOutput)
				+ this->numHidden + this->numOutput;
    double* initialWeights = new double[numWeights];
    for (int i = 0; i < numWeights; i++)
            initialWeights[i] = (0.001 - 0.0001) * ((double)rand() / (RAND_MAX))+ 0.0001;
            			
	this->SetWeights(initialWeights);
			
	delete[] initialWeights;
}


void Rprop::TrainRPROP(int numTrain, double **trainData, double **patternData, int maxEpochs, double *out_wts) // using RPROP
{
    // there is an accumulated gradient and a previous gradient for each weight and bias
    double* hGradTerms = new double[numHidden]; // intermediate val for h-o weight gradients
    double* oGradTerms = new double[numOutput]; // output gradients

    
    double* oBiasGradsAcc = new double[numOutput];
    double* hBiasGradsAcc = new double[numHidden];    
    double* oPrevBiasGradsAcc = new double[numOutput];
    double* hPrevBiasGradsAcc = new double[numHidden];

    // must save previous weight deltas
    double** hoPrevWeightDeltas = new double*[numHidden];
	double** ihPrevWeightDeltas = new double*[numInput];;
	

	double** hoWeightGradsAcc	  = new double*[numHidden];; ;	
    double** ihWeightGradsAcc	  = new double*[numInput];;;	
	double** hoPrevWeightGradsAcc = new double*[numHidden];;;	
    double** ihPrevWeightGradsAcc = new double*[numInput];;;
	

	for(int i = 0;i < numHidden;i++){
		hoPrevWeightDeltas[i]	= new double[numOutput];
		hoWeightGradsAcc[i]		= new double[numOutput];
		hoPrevWeightGradsAcc[i]	= new double[numOutput];
		
	}
	
	for(int i = 0;i < numInput;i++){
		ihPrevWeightDeltas[i]		= new double[numHidden];
		ihWeightGradsAcc[i]			= new double[numHidden];
		ihPrevWeightGradsAcc[i]		= new double[numHidden];
		
	}

	MakeMatrix(hoWeightGradsAcc,numHidden, numOutput, 0.0); // accumulated over all training data
	MakeMatrix(ihWeightGradsAcc,numInput, numHidden, 0.0);
	MakeMatrix(hoPrevWeightGradsAcc,numHidden, numOutput, 0.0); // accumulated, previous iteration
	MakeMatrix(ihPrevWeightGradsAcc,numInput, numHidden, 0.0);


	MakeMatrix(hoPrevWeightDeltas,numHidden, numOutput, 0.01);
	MakeMatrix(ihPrevWeightDeltas, numInput, numHidden, 0.01);
	
	double* oPrevBiasDeltas = new double[numOutput];		
    double* hPrevBiasDeltas = new double[numHidden];


	MakeVector(oPrevBiasDeltas, numOutput, 0.01);	
	MakeVector(hPrevBiasDeltas, numHidden, 0.01);

    double etaPlus = 1.2; // values are from the paper
    double etaMinus = 0.5;
    double deltaMax = 50.0;
    double deltaMin = 1.0E-6;

    int epoch = 0;

	this->InitializeWeights(); // all weights and biases

    while (epoch < maxEpochs)
	{
		if(handler->GetLearningStopSignal()) 
		{
			break;
		}
        ++epoch;

        if (epoch % 100 == 0 && epoch != maxEpochs)
        {
			int numWeights = (numInput * numHidden) + (numHidden * numOutput) + numHidden + numOutput;
			double *result = new double[numWeights];
            this->GetWeights(result);			

			double err = MeanSquaredError(numTrain, trainData, patternData,  result);

			delete[] result;

					
			cout << "( epoch, err ) = " << epoch  << " :: " <<  err << endl;
			
			this->learning_convergence_error_rate = err;
            //Console.WriteLine("epoch = " + epoch + " err = " + err.ToString("F4"));
        }


		for(int i=0; i<numHidden; i++)
			for(int j=0;j<numOutput; j++)
				hoWeightGradsAcc [i][j]=0;

		for(int i=0; i<numInput; i++)
			for(int j=0;j<numHidden; j++)
				ihWeightGradsAcc [i][j]=0;

		for(int i=0; i<numOutput; i++) oBiasGradsAcc[i]=0;
		for(int i=0; i<numHidden; i++) hBiasGradsAcc[i]=0;

        // 1. compute and accumulate all gradients
        //ZeroOut(numHidden, numOutput, hoWeightGradsAcc); // zero-out values from prev iteration
        //ZeroOut( numInput, numHidden, ihWeightGradsAcc);
        //ZeroOut_Array(oBiasGradsAcc, numOutput);
        //ZeroOut_Array(hBiasGradsAcc, numHidden);

        double *xValues = new double[numInput]; // inputs
        double *tValues = new double[numOutput]; // target values
        for (int row = 0; row < numTrain; row++)  // walk thru all training data
        {
            // no need to visit in random order because all rows processed before any updates ('batch')
										
			for(int j=0; j<numInput; j++)  xValues[j]= trainData[row][j];
			for(int j=0; j<numOutput; j++) tValues[j]= patternData[row][j];				

            //Array.Copy(trainData[row], xValues, numInput); // get the inputs
            //Array.Copy(trainData[row], numInput, tValues, 0, numOutput); // get the target values

            ComputeOutputs(xValues, outputs); // copy xValues in, compute outputs using curr weights (and store outputs internally)

            // compute the h-o gradient term/component as in regular back-prop
            // this term usually is lower case Greek delta but there are too many other deltas below
            for (int i = 0; i < numOutput; i++)
            {
                double derivative = (1 - outputs[i]) * outputs[i]; // derivative of softmax = (1 - y) * y (same as log-sigmoid)
                oGradTerms[i] = derivative * (outputs[i] - tValues[i]); // careful with O-T vs. T-O, O-T is the most usual
            }

            // compute the i-h gradient term/component as in regular back-prop
            for (int i = 0; i < numHidden; i++)
            {
                //   double derivative = (1 - hOutputs[i]) * (1 + hOutputs[i]); // derivative of tanh = (1 - y) * (1 + y)
				double derivative = (1 - hOutputs[i]) * hOutputs[i]; // derivative of tanh = (1 - y) * (1 + y)
                double sum = 0.0;
                for (int j = 0; j < numOutput; j++) // each hidden delta is the sum of numOutput terms
                {
                    double x = oGradTerms[j] * hoWeights[i][j];
                    sum += x;
                }
                hGradTerms[i] = derivative * sum;
            }

            // add input to h-o component to make h-o weight gradients, and accumulate
            for (int i = 0; i < numHidden; i++)
            {
                for (int j = 0; j < numOutput; j++)
                {
                    double grad = oGradTerms[j] * hOutputs[i];
                    hoWeightGradsAcc[i][j] += grad;
                }
            }

            // the (hidden-to-) output bias gradients
            for (int i = 0; i < numOutput; i++)
            {
                double grad = oGradTerms[i] * 1.0; // dummy input
                oBiasGradsAcc[i] += grad;
            }

            // add input term to i-h component to make i-h weight gradients and accumulate
            for (int i = 0; i < numInput; i++)
            {
                for (int j = 0; j < numHidden; j++)
                {
                    double grad = hGradTerms[j] * inputs[i];
                    ihWeightGradsAcc[i][j] += grad;
                }
            }

            // the (input-to-) hidden bias gradient
            for (int i = 0; i < numHidden; i++)
            {
                double grad = hGradTerms[i] * 1.0;
                hBiasGradsAcc[i] += grad;
            }
        } // each row
        // end compute all gradients

        // update all weights and biases (in any order)

        // update input-hidden weights
        double delta = 0.0;

        for (int i = 0; i < numInput; i++)
        {
            for (int j = 0; j < numHidden; j++)
            {
                if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] > 0) // no sign change, increase delta
                {
                    delta = ihPrevWeightDeltas[i][j] * etaPlus; // compute delta
                    if (delta > deltaMax) delta = deltaMax; // keep it in range
                    double tmp = -Sign(ihWeightGradsAcc[i][j]) * delta; // determine direction and magnitude
                    ihWeights[i][j] += tmp; // update weights
                }
                else if (ihPrevWeightGradsAcc[i][j] * ihWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
                {
                    delta = ihPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved for later)
                    if (delta < deltaMin) delta = deltaMin; // keep it in range

                    ihWeights[i][j] -= ihPrevWeightDeltas[i][j]; // revert to previous weight
                    ihWeightGradsAcc[i][j] = 0; // forces next if-then branch, next iteration

				//	double tmp = -Sign(ihWeightGradsAcc[i][j]) * delta; // determine direction and magnitude
                //    ihWeights[i][j] += tmp; // update weights
                }
                else // this happens next iteration after 2nd branch above (just had a change in gradient)
                {
                    delta = ihPrevWeightDeltas[i][j]; // no change to delta
                    // no way should delta be 0 . . . 
                    double tmp = -Sign(ihWeightGradsAcc[i][j]) * delta; // determine direction
                    ihWeights[i][j] += tmp; // update
                }
                //Console.WriteLine(ihPrevWeightGradsAcc[i][j] + " " + ihWeightGradsAcc[i][j]); Console.ReadLine();

                ihPrevWeightDeltas[i][j] = delta; // save delta
                ihPrevWeightGradsAcc[i][j] = ihWeightGradsAcc[i][j]; // save the (accumulated) gradient
            } // j
        } // i

        // update (input-to-) hidden biases
        for (int i = 0; i < numHidden; i++)
        {
            if (hPrevBiasGradsAcc[i] * hBiasGradsAcc[i] > 0) // no sign change, increase delta
            {
                delta = hPrevBiasDeltas[i] * etaPlus; // compute delta
                if (delta > deltaMax) delta = deltaMax;
                double tmp = -Sign(hBiasGradsAcc[i]) * delta; // determine direction
                hBiases[i] += tmp; // update
            }
            else if (hPrevBiasGradsAcc[i] * hBiasGradsAcc[i] < 0) // grad changed sign, decrease delta
            {
                delta = hPrevBiasDeltas[i] * etaMinus; // the delta (not used, but saved later)
                if (delta < deltaMin) delta = deltaMin;
                hBiases[i] -= hPrevBiasDeltas[i]; // revert to previous weight
                hBiasGradsAcc[i] = 0; // forces next branch, next iteration
				
				//double tmp = -Sign(hBiasGradsAcc[i]) * delta; // determine direction
                //hBiases[i] += tmp; // update
			}
            else // this happens next iteration after 2nd branch above (just had a change in gradient)
            {
                delta = hPrevBiasDeltas[i]; // no change to delta

                if (delta > deltaMax) delta = deltaMax;
                else if (delta < deltaMin) delta = deltaMin;
                // no way should delta be 0 . . . 
                //double tmp = -Math.Sign(hBiasGradsAcc[i]) * delta; // determine direction
				double tmp = -Sign(hBiasGradsAcc[i]) * delta; ; // determine direction
                hBiases[i] += tmp; // update
            }
            hPrevBiasDeltas[i] = delta;
            hPrevBiasGradsAcc[i] = hBiasGradsAcc[i];
        }

        // update hidden-to-output weights
        for (int i = 0; i < numHidden; i++)
        {
            for (int j = 0; j < numOutput; j++)
            {
                if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] > 0) // no sign change, increase delta
                {
					delta = hoPrevWeightDeltas[i][j] * etaPlus; // compute delta
					if (delta > deltaMax) delta = deltaMax;
					double tmp = -Sign(hoWeightGradsAcc[i][j]) * delta; // determine direction
					hoWeights[i][j] += tmp; // update
                }
                else if (hoPrevWeightGradsAcc[i][j] * hoWeightGradsAcc[i][j] < 0) // grad changed sign, decrease delta
                {
                    delta = hoPrevWeightDeltas[i][j] * etaMinus; // the delta (not used, but saved later)
                    if (delta < deltaMin) delta = deltaMin;
                    hoWeights[i][j] -= hoPrevWeightDeltas[i][j]; // revert to previous weight
                    hoWeightGradsAcc[i][j] = 0; // forces next branch, next iteration

					//double tmp = -Sign(hoWeightGradsAcc[i][j]) * delta; // determine direction
					//hoWeights[i][j] += tmp; // update
                }
                else // this happens next iteration after 2nd branch above (just had a change in gradient)
                {
                    delta = hoPrevWeightDeltas[i][j]; // no change to delta
                    // no way should delta be 0 . . . 
                    double tmp = -Sign(hoWeightGradsAcc[i][j]) * delta; // determine direction
                    hoWeights[i][j] += tmp; // update
                }
                hoPrevWeightDeltas[i][j] = delta; // save delta
                hoPrevWeightGradsAcc[i][j] = hoWeightGradsAcc[i][j]; // save the (accumulated) gradients
            } // j
        } // i

        // update (hidden-to-) output biases
        for (int i = 0; i < numOutput; i++)
        {
            if (oPrevBiasGradsAcc[i] * oBiasGradsAcc[i] > 0) // no sign change, increase delta
            {
                delta = oPrevBiasDeltas[i] * etaPlus; // compute delta
                if (delta > deltaMax) delta = deltaMax;
                double tmp = -Sign(oBiasGradsAcc[i]) * delta; // determine direction
                oBiases[i] += tmp; // update
            }
            else if (oPrevBiasGradsAcc[i] * oBiasGradsAcc[i] < 0) // grad changed sign, decrease delta
            {
                delta = oPrevBiasDeltas[i] * etaMinus; // the delta (not used, but saved later)
                if (delta < deltaMin) delta = deltaMin;
                oBiases[i] -= oPrevBiasDeltas[i]; // revert to previous weight
                oBiasGradsAcc[i] = 0; // forces next branch, next iteration

				//double tmp = -Sign(oBiasGradsAcc[i]) * delta; // determine direction
                //oBiases[i] += tmp; // update

            }
            else // this happens next iteration after 2nd branch above (just had a change in gradient)
            {
                delta = oPrevBiasDeltas[i]; // no change to delta
                // no way should delta be 0 . . . 
                double tmp = -Sign(hBiasGradsAcc[i]) * delta; // determine direction
                oBiases[i] += tmp; // update
            }
            oPrevBiasDeltas[i] = delta;
            oPrevBiasGradsAcc[i] = oBiasGradsAcc[i];
        }
		delete[] xValues;
		delete[] tValues;
    } // while
			
	if(out_wts!=NULL)
	{
        this->GetWeights(out_wts);			
	}



	for(int i = 0;i < numHidden;i++){
		delete[] hoWeightGradsAcc [i];
	}
	for(int i = 0;i < numInput;i++){
		delete[] ihWeightGradsAcc [i];
	}
	delete[] hoWeightGradsAcc;
	delete[] ihWeightGradsAcc ;


	for(int i = 0;i < numHidden;i++){
		delete[] hoPrevWeightGradsAcc [i];
	}
	for(int i = 0;i < numInput;i++){
		delete[] ihPrevWeightGradsAcc [i];
	}
	delete[] hoPrevWeightGradsAcc ;
	delete[] ihPrevWeightGradsAcc ;


	for(int i = 0;i < numHidden;i++){
		delete[] hoPrevWeightDeltas  [i];
	}
	for(int i = 0;i < numInput;i++){
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
			
			 
} 
		

bool Rprop::saveWeight(char* filename)
{
	//open file for reading
	fstream outputFile;
	outputFile.open(filename, ios::out);

	if ( outputFile.is_open() )
	{
		outputFile.precision(50);		

		int numWeights = (numInput * numHidden) + (numHidden * numOutput) + numHidden + numOutput;
		double *weights =  new double[numWeights];
		GetWeights(weights);
		//output weights
		for ( int i=0; i < numWeights; i++ ) 
		{
			if(i==numWeights-1)
				outputFile << weights[i] << endl;
			else
				outputFile << weights[i] << ",";
		}

		//print success
		cout << endl << "Neuron weights saved to '" << filename << "'" << endl;

		delete[] weights;
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

void Rprop::inputMap(std::map<int, double*> waveform_character, int num_train, int num_character, int num_target)
{
	int i,j ;
	for( i=0; i< num_train; i++)
	{
		int index=0;
		double* temp = waveform_character[i];
		for(j=0; j< num_character; j++) 
		{
			input[i][j] = temp[j];
		}		
		for(j=num_character; j<num_character+num_target; j++)
		{
			target_output[i][index++] = temp[j];
		}		
	}
}

void Rprop::Train()
{
			
	int i, j;
	double **input_part			= new double*[11];
	double **output_part		= new double*[11];
	double *output_ref			= new double[numOutput]; // 161015 
	 
			
	//트레이닝

	TrainRPROP(num_train, input ,target_output, 15000, NULL);
	 

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );		

	saveWeight(fileloc);	 
	delete[] input_part;
	delete[] output_part;

	delete[] output_ref;
}


void Rprop::inputfile(char *filename, int num_train)
{
	fstream inputFile;
	inputFile.open(filename, ios::in);	

	//csv 파일 읽기
	if ( inputFile.is_open() )
	{
		string line = "";
		int i=0;
		//read data
		while ( !inputFile.eof() )
		{

			getline(inputFile, line);				

			//process line
			if (line.length() > 2 ) processLine(line, numInput, numOutput, input[i], target_output[i]);
			i++;
			if(i>=num_train) break;
		}		
	}
	inputFile.close();
}

void Rprop::inputfile_phase_mag(char *filename, int num_train)
{
	fstream inputFile;
	inputFile.open(filename, ios::in);	

	//csv 파일 읽기
	if ( inputFile.is_open() )
	{
		string line = "";
		int i=0;
		//read data
		while ( !inputFile.eof() )
		{

			getline(inputFile, line);				

			//process line
			if (line.length() > 2 ) processLine(line, numInput, numOutput, input[i], target_output[i]);
		/*	double tran_input[2000];
			for(int j=0;j<numInput/2;j++)
			{ 
				double fPhase;
				fPhase = input[i][j];
				// 스케일링
				fPhase = (fPhase)/360+0.5; 
				tran_input[j*2] = fPhase;
			}
			for(int j=0;j<numInput/2;j++)
			{
				double fMag; 
				fMag = input[i][j + numInput/2];
				fMag = (fMag + 70) / 140.0;
				if(fMag>1) fMag = 1;
				if(fMag<0) fMag = 0;

				tran_input[j*2+1] = fMag;
			}
			for(int j=0;j<numInput;j+=2)
			{
				// phase check
				if(tran_input[j]<0 || tran_input[j]>1)
				{
					AfxMessageBox(L"안대1");
				}
				// mag check
				if(tran_input[j+1]<0 || tran_input[j+1]>1)
				{
					AfxMessageBox(L"안대2");
				}
				input[i][j] = tran_input[j];
				input[i][j+1] = tran_input[j+1];
			}*/
			
			for(int j=0; j< numInput; j++)
			{

				input[i][j] = (input[i][j]+2.5)/5.0;
				//input[i][j] = (input[i][j]) / 360.0;
				//input[i][j] += 0.5;
			}
			
			for(int j=0;j<numOutput;j++)
			{
				target_output[i][j] = (target_output[i][j] + 10.0)/20.0;
			}
			i++;
			if(i>=num_train) break;
		}		
	}
	inputFile.close();
}

void Rprop::inputfile_phase(char *filename, int num_train)
{
	fstream inputFile;
	inputFile.open(filename, ios::in);	

	//csv 파일 읽기
	if ( inputFile.is_open() )
	{
		string line = "";
		int i=0;
		//read data
		while ( !inputFile.eof() )
		{

			getline(inputFile, line);				

			//process line
			if (line.length() > 2 ) processLine(line, numInput, numOutput, input[i], target_output[i]);
			for(int j=0;j<numInput;j+=2)
			{
				double fReal = input[i][j];
				double fImage = input[i][j+1];

				double fMag;
				double fPhase;
				fMag = - 20.0 * log(sqrt(pow(fReal,2)+pow(fImage,2))) / log(10.0);
				fPhase = 180/M_PI*atan2(fImage, fReal);
				if(fPhase>180) fPhase -= 360.0;
				fPhase /= 360.0;
				input[i][j] = fPhase;
				input[i][j+1] = fPhase;
			}
			i++;
			if(i>=num_train) break;
		}		
	}
	inputFile.close();
}

void Rprop::processLine( string &line , int nInputs, int nTargets, double *input, double *output)
{
	//create new pattern and target
	double* pattern = new double[nInputs];
	double* target  = new double[nTargets];

	//store inputs		
	char* cstr = new char[line.size()+1];
	char* t;
	strcpy(cstr, line.c_str());
			
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
	 
	// 161012
	delete[] cstr;
	delete[] pattern;
	delete[] target;



}

void Rprop::Train_RT()
{
	//int index[] = { 17, 17,17, 13,18, 16,17,13, 17,14, 17, 9, 17, 16, 17, 17, 17, 16, 13};
	int index[] = { 11, 11,11, 11, 11, 11,11,11, 11,11, 11, 11, 11, 11, 11, 11, 11, 11, 11};
	
	int i, j;
	int index_sum =209;
	int output_count = numOutput; // 161015
	double **input_part			= new double*[100];
	double **output_part		= new double*[100];
	double *output_ref			= new double[numOutput]; // 161015

	//numOutput = 1;

	for(i=numOutput;i>=1; i--) // 161015
	{
		int index_count = 0;
		
		
		cout << index_sum-index[i-1] << " - " << index_sum<< endl;
		for(j=index_sum-index[i-1]; j< index_sum; j++)
		{
			//memcpy(test_array,input[j],sizeof(test_array));

			output_part[index_count]			= this->target_output[j];
			//output_part[index_count]			= new double[1];
			//output_part[index_count][0]			= target_output[j][i-1];
			input_part[index_count++]			= input[j];

		}

		int trainNum = index[i-1];
		index_sum -= index[i-1];

		TrainRPROP(trainNum, input_part, output_part, 500, NULL);		
		 
		/*ComputeOutputs(input[0],output_ref);


		if(i>1){ 
			for(j=index_sum-index[i-1]; j< index_sum; j++)
			{
				target_output[j][i-1] = target_output[j][i-1] + (output_ref[0]-0.5);						
			}
		}
		*/
		char fileloc[1024];
		CopyCStringToChar( project->GetWeightRTFileLoc(i), fileloc );		
		saveWeight(fileloc);

	} 
	delete[] output_ref;
	delete[] input_part;
	delete[] output_part;
/*				
	for(int i = 0;i < num_train;i++){
		delete[] input[i];
		delete[] target_output[i];
	}
	delete[] input;
	delete[] target_output;
	*/		

}

void Rprop::Train_RT(int *index, int num_pole)
{
	//int index[] = { 17, 17,17, 13,18, 16,17,13, 17,14, 17, 9, 17, 16, 17, 17, 17, 16, 13};
	
	int i, j;
	int index_sum =1;
	int output_count = num_pole;
	double **input_part			= new double*[100];
	double **output_part		= new double*[100];
	double *output_ref			= new double[num_pole];

	this->numOutput = 1;

	for(i=1;i<=num_pole; i++)
	{
		int index_count = 0;
		 
		cout << index_sum << " - " << index_sum + index[i-1]<< endl;
		for(j=index_sum; j< index_sum+index[i-1]; j++)
		{
			//output_part[index_count]			= this->target_output[j];
			output_part[index_count]			= new double[1];
			output_part[index_count][0]			= target_output[j][i-1];
			input_part[index_count++]			= input[j];

		}

		int trainNum = index[i-1];
		index_sum += (index[i-1]);

		TrainRPROP(trainNum, input_part, output_part, 600, NULL);		
		  
		char fileloc[1024];
		CopyCStringToChar( project->GetWeightRTFileLoc(i), fileloc );		
		saveWeight(fileloc);

	} 

	delete[] output_ref;
	delete[] input_part;
	delete[] output_part;
/*				
	for(int i = 0;i < num_train;i++){
		delete[] input[i];
		delete[] target_output[i];
	}
	delete[] input;
	delete[] target_output;
	*/		

}


void Rprop::Train(int *pole_file_count, int num_pole)
{
			
	int i, j; 

			
	//트레이닝

	TrainRPROP(num_train, input ,target_output, 15000, NULL);
	 

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );		

	saveWeight(fileloc);	 
}

bool Rprop::loadWeights(char* filename)
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
		if ( weights.size() != ( numOutput * (numHidden+1) + numHidden * (numInput+1 )) ) 
		{
			cout << endl << "Error - Incorrect number of weights in input file: " << filename << endl;

			//close file
			inputFile.close();

			return false;
		}
		else
		{
			double *weight = new double[weights.size()];

			for(int i=0; i<weights.size(); i++)		weight[i] = weights[i];
					
			//set weights					
			SetWeights(weight);

			//print success
			cout << endl << "Neuron weights loaded successfuly from '" << filename << "'" << endl;

			//close file
			inputFile.close();

			delete[] weight;
			return true;
		}		
	}
	else 
	{
		cout << endl << "Error - Weight input file '" << filename << "' could not be opened: " << endl;
		return false;
	}

}

void Rprop::ZeroOut(int numRows, int numCols, double **matrix)
{

    for (int i = 0; i < numRows; i++)
        for (int j = 0; j < numCols; j++)
            matrix[i][j] = 0.0;
}

void Rprop::ZeroOut_Array(double *array, int array_length) 
{
    for (int i = 0; i < array_length; i++)
        array[i] = 0.0;
}

void Rprop::SetWeights(double *weights)
{			
    // copy weights and biases in weights[] array to i-h weights, i-h biases, h-o weights, h-o biases
    int numWeights = (numInput * numHidden) + (numHidden * numOutput) + numHidden + numOutput; 

    int k = 0; // points into weights param

    for (int i = 0; i < numInput; i++)
        for (int j = 0; j < numHidden; j++)
            ihWeights[i][j] = weights[k++];
    for (int i = 0; i < numHidden; i++)
        hBiases[i] = weights[k++];
    for (int i = 0; i < numHidden; i++)
        for (int j = 0; j < numOutput; j++)
            hoWeights[i][j] = weights[k++];
    for (int i = 0; i < numOutput; i++)
        oBiases[i] = weights[k++];
}

void Rprop::GetWeights(double *result)
{
    int numWeights = (numInput * numHidden) + (numHidden * numOutput) + numHidden + numOutput; 
    int k = 0;
    for (int i = 0; i < numInput; i++)
        for (int j = 0; j < numHidden; j++)
			result[k++] = ihWeights[i][j];
    for (int i = 0; i < numHidden; i++)
        result[k++] = hBiases[i];
    for (int i = 0; i < numHidden; i++)
        for (int j = 0; j < numOutput; j++)
            result[k++] = hoWeights[i][j];
    for (int i = 0; i < numOutput; i++)
        result[k++] = oBiases[i];
  //  return result;
}



void Rprop::ComputeOutputs(double* xValues, double* retResult)
{
    double* hSums = new double[numHidden]; // hidden nodes sums scratch array
    double* oSums = new double[numOutput]; // output nodes sums

	for (int j = 0; j < numHidden; j++)  hSums[j]=0;
	for (int j = 0; j < numOutput; j++)  oSums[j]=0;
		
    for (int i = 0; i < numInput; i++) // copy x-values to inputs
        this->inputs[i] = xValues[i];
    // note: no need to copy x-values unless you implement a ToString and want to see them.
    // more efficient is to simply use the xValues[] directly.

    for (int j = 0; j < numHidden; j++)  // compute i-h sum of weights * inputs
        for (int i = 0; i < numInput; i++)
            hSums[j] += xValues[i] * ihWeights[i][j]; // note +=

    for (int i = 0; i < numHidden; i++)  // add biases to input-to-hidden sums
        hSums[i] += hBiases[i];

			
    for (int i = 0; i < numHidden; i++)   // apply activation
        hOutputs[i] = HyperTan(hSums[i]); // hard-coded

    for (int j = 0; j < numOutput; j++)   // compute h-o sum of weights * hOutputs
        for (int i = 0; i < numHidden; i++)
            oSums[j] += hOutputs[i] * hoWeights[i][j];

    for (int i = 0; i < numOutput; i++)  // add biases to input-to-hidden sums
        oSums[i] += oBiases[i];
     
	if(retResult!=NULL)
	{		
		for(int j=0; j<numOutput; j++)  retResult[j]= HyperTan(oSums[j]);
		
	}
	//Array.Copy(this->outputs, retResult, numOutput);			
			
	delete[] oSums;
	delete[] hSums;

    // return retResult;
}

double Rprop::HyperTan(double x)
{
    //  if (x < -20.0) return -1.0; 
    // else if (x > 20.0) return 1.0;
    //else return tanh(x);
	return 1 / (1 + exp(-x));
			
}

double*  Rprop::Softmax(double *oSums)
{
    // does all output nodes at once so scale doesn't have to be re-computed each time
    // determine max output-sum
    /*  double max = oSums[0];
    for (int i = 0; i < numOutput; ++i)
        if (oSums[i] > max) max = oSums[i];

    // determine scaling factor -- sum of exp(each val - max)
    double scale = 0.0;
    for (int i = 0; i < numOutput; ++i)
        scale += (double)exp(oSums[i] - max);

    double *result = new double[numOutput];
    for (int i = 0; i < numOutput; ++i)
        result[i] = (double)exp(oSums[i] - max) / scale;*/
	double *result = new double[numOutput];
	for (int i = 0; i < numOutput; i++)
		result[i] = HyperTan(oSums[i]);
			
    return result; // now scaled so that xi sum to 1.0
}

double Rprop::Accuracy(int numTest, double** testData, double* weights)
{
			
    SetWeights(weights);
    // percentage correct using winner-takes all
    int numCorrect = 0;
    int numWrong = 0;
    double* xValues = new double[numInput]; // inputs
    double* tValues = new double[numOutput]; // targets
    // 161012 comment 처리
	// double* yValues; // computed Y

    for (int i = 0; i < numTest; i++)
    {				

        //Array.Copy(testData[i], xValues, numInput); // parse data into x-values and t-values
        //Array.Copy(testData[i], numInput, tValues, 0, numOutput);

		for(int j=0; j<numInput; j++)  xValues[j]= testData[i][j];
		for(int j=0; j<numOutput; j++) tValues[j]= testData[i][numInput+j];				
		
		double* yValues = new double[numOutput];
		ComputeOutputs(xValues, yValues);
        int maxIndex = this->MaxIndex(numOutput, yValues); // which cell in yValues has largest value?

        if (tValues[maxIndex] == 1.0) // ugly. consider AreEqual(double x, double y, double epsilon)
            ++numCorrect;
        else
            ++numWrong;
		delete[] yValues;
    }
	// 161012
	delete[] xValues;
	delete[] tValues;

    return (numCorrect * 1.0) / (numCorrect + numWrong); // ugly 2 - check for divide by zero
}

double Rprop::MeanSquaredError(int numTrain, double** trainData, double **paternData, double* weights)
{
    this->SetWeights(weights); // copy the weights to evaluate in
			
    double* xValues = new double[numInput]; // inputs
    double* tValues = new double[numOutput]; // targets
    double sumSquaredError = 0.0;
    for (int i = 0; i < numTrain; i++) // walk through each training data item
    {
        // following assumes data has all x-values first, followed by y-values!								
		for(int j=0; j<numInput; j++)  xValues[j]= trainData[i][j];
		for(int j=0; j<numOutput; j++) tValues[j]= paternData[i][j];				

		double* yValues = new double[numOutput];

        ComputeOutputs(xValues, yValues);
        for (int j = 0; j < numOutput; j++)
            sumSquaredError += ((yValues[j] - tValues[j]) * (yValues[j] - tValues[j]));
		delete[] yValues;
    }	
	// 161012
	delete[] xValues;
	delete[] tValues;

    return sumSquaredError / numTrain;
}
		
int Rprop::MaxIndex(int numVector, double *vector) // helper for Accuracy()
{
    // index of largest value
    int bigIndex = 0;
    double biggestVal = vector[0];
    for (int i = 0; i < numVector; i++)
    {
        if (vector[i] > biggestVal)
        {
            biggestVal = vector[i];	
            bigIndex = i;
        }
    }
    return bigIndex;
} 