#include "StdAfx.h"
#include "FilterTuner.h"


CFilterTuner::CFilterTuner(void)
{
}


CFilterTuner::~CFilterTuner(void)
{
}

void CFilterTuner::predict(double* main_data, double* sub_data, void* result)
{
	double* main_data_freq;
	double* main_data_real;
	double* main_data_image;
	double* main_data_mag;
	double* main_data_phase;

	// 아마 아래 부분 정상 동작 안할 듯 
	main_data_freq	= &main_data[2048*0];
	main_data_real	= &main_data[2048*0];
	main_data_image	= &main_data[2048*2];
	main_data_mag	= &main_data[2048*3];
	main_data_phase	= &main_data[2048*4];

	int i,j;
	for(i=0;i<5;i++)
	{
		CString pp = L"";
		CString pp_sub = L"";
		for(j=0;j<10;j++)
		{
			// i(0) -> freq;
			// i(1) -> real;
			// ...
			pp_sub.Format(L"%8d",main_data[i*2048 + j]);
			if(i==0) 
			{
				pp_sub.Format(L"%8d",main_data_freq[j]);
			}
			pp += pp_sub;
		}
		OutputDebugString(pp + L"\r\n");

	}
}
