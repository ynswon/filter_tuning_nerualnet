#pragma once

#include<list>
#include<map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

#define USE_VISA

#ifdef USE_VISA
#pragma comment(lib, "visa32.lib")
#include "visa.h"
#else
#define ViSession int*
#define ViPUInt32 int*
#endif

////cjh 계측기 추가 부분 .
#define Rohde_AND_Schwarz 1
#define AgilentENA 2
#define AgilentPNA 3
#define AgilentFieldFox 4




#define IDX_DATA_S11 0x00
#define IDX_DATA_S12 0x01
#define IDX_DATA_S13 0x02
#define IDX_DATA_S14 0x03
#define IDX_DATA_S15 0x04
#define IDX_DATA_S16 0x05
#define IDX_DATA_S17 0x06
#define IDX_DATA_S18 0x07

#define IDX_DATA_S21 0x10
#define IDX_DATA_S22 0x11
#define IDX_DATA_S23 0x12
#define IDX_DATA_S24 0x13
#define IDX_DATA_S25 0x14
#define IDX_DATA_S26 0x15
#define IDX_DATA_S27 0x16
#define IDX_DATA_S28 0x17

#define IDX_DATA_S31 0x20
#define IDX_DATA_S32 0x21
#define IDX_DATA_S33 0x22
#define IDX_DATA_S34 0x23
#define IDX_DATA_S35 0x24
#define IDX_DATA_S36 0x25
#define IDX_DATA_S37 0x26
#define IDX_DATA_S38 0x27

#define IDX_DATA_S41 0x30
#define IDX_DATA_S42 0x31
#define IDX_DATA_S43 0x32
#define IDX_DATA_S44 0x33
#define IDX_DATA_S45 0x34
#define IDX_DATA_S46 0x35
#define IDX_DATA_S47 0x36
#define IDX_DATA_S48 0x37

#define IDX_DATA_S51 0x40
#define IDX_DATA_S52 0x41
#define IDX_DATA_S53 0x42
#define IDX_DATA_S54 0x43
#define IDX_DATA_S55 0x44
#define IDX_DATA_S56 0x45
#define IDX_DATA_S57 0x46
#define IDX_DATA_S58 0x47

#define IDX_DATA_S61 0x50
#define IDX_DATA_S62 0x51
#define IDX_DATA_S63 0x52
#define IDX_DATA_S64 0x53
#define IDX_DATA_S65 0x54
#define IDX_DATA_S66 0x55
#define IDX_DATA_S67 0x56
#define IDX_DATA_S68 0x57

#define IDX_DATA_S71 0x60
#define IDX_DATA_S72 0x61
#define IDX_DATA_S73 0x62
#define IDX_DATA_S74 0x63
#define IDX_DATA_S75 0x64
#define IDX_DATA_S76 0x65
#define IDX_DATA_S77 0x66
#define IDX_DATA_S78 0x67

#define IDX_DATA_S81 0x70
#define IDX_DATA_S82 0x71
#define IDX_DATA_S83 0x72
#define IDX_DATA_S84 0x73
#define IDX_DATA_S85 0x74
#define IDX_DATA_S86 0x75
#define IDX_DATA_S87 0x76
#define IDX_DATA_S88 0x77


#define IDX2_FREQ	0
#define IDX2_REAL	1
#define IDX2_IMAGE	2
#define IDX2_MAG	3
#define IDX2_PHASE	4

#define TOTAL_PRED_RANGE 20
#define TOTAL_SWEEP_RANGE 20
#define TOTAL_PORT 8
#define MY_INT64_MAX 9223372036854775807i64
#define MY_INT64_MIN (-9223372036854775807i64 - 1)

struct SFilterSweepItem
{
public:
	int nExist;
	__int64 nStartFreq; // in hz 
	__int64 nStopFreq; // in hz 
	int nPoint; // point
	int nInputFilter; // in hz
	int nPower; // in dbm
};
struct SFilterPredItem
{
public:
	int nExist;
	__int64 nStartFreq; // in hz 
	__int64 nStopFreq; // in hz
};

struct SFilterSweepOption {
	int aTraceOption[8][8];
	int aTraceOrderMapping[8][8];
	std::list<int> aTraceOrderInt;
	std::list<CString> aTraceOrderString;
	CString sReflection;
	int nReflectionLearningType;
	int nReflectionIdx;

	// ""면 Transmission이 없는 것임
	CString sTransmission;
	int nTransmissionLearningType;
	int nTransmissionIdx;
	int nUseRangePred;


}; 



class CNetworkAnalyzerCtr
{
public:
	CNetworkAnalyzerCtr(void);
	~CNetworkAnalyzerCtr(void);

	int open();
	void close();
	void disp_off(ViSession* vi);
	void disp_on(ViSession* vi);
	// version 1
	void fetch();
	void read_master();

	// version 2 data from s11, s22 and s12
	int fetch_ver2(int operation_mode = 0, int pole = 0, int degree = 0, bool use_legacy_compatibility = true);
/*
	void CopyPresentDataTo(int& count, boolean* data_exist, double (*mymy)[5][2048]);
	void CopyPresentDataTo(int& count, int idx, double (*mymy)[2048]);
*/
	void meas(int mode, int& count, double (*waveform)[5][2048] ,int tracecount = 0) ;

	void mywrite(const char* mystr, ViPUInt32 retCnt);

	void initialize_model_builder();
	void initialize_model_builderENA();
	void initialize_model_builderRohde_AND_Schwarz();
	void get_final_waveform(
		int wafeform_type, 
		int max_count, 
		double* data_freq,
		double* data_real,
		double* data_image,
		double* data_mag,
		double* data_phase);

	int GetTotalPoints();
	std::list<int> GetTraceOrder();
private:
	int _is_opened;
	int Typenetworkanal;// cjh  계측기 선택 부분 .. 실제 사용하는 계측기 번호 보면 됨.. define
	ViSession defaultRM;
	ViSession vi;

public:
	int GetDataFetchSqc()
	{
		return data_fetch_sqc;
	}
	int GetIndivGetDataFetchSqc(int idx1)
	{
		return data_sqc[idx1];
	}
	int GetIndivCnt(int idx1)
	{
		return data_cnt[idx1];
	}
	double GetData(int idx1, int idx2, int idx3)
	{
		return present_data[idx1][idx2][idx3];
	}
private:
	int count_freq;
	int count_data;
	int OPERATION_MODE;
	CString project_name;
	CString filter_name;
	CString tuning_phase_name;
	int total_poles;
	CRITICAL_SECTION cs;
	int data_fetch_sqc;
	double present_data[128][5][2048];
	int present_data_count;

	int data_sqc[128];
	int data_cnt[128];
	 
//	char general_buffer[8192];
	char general_buffer[1024*1024];

	double data_freq[2048];
	double data_real[2048];
	double data_image[2048];
	double data_mag[2048];
	double data_phase[2048];


	double data_master_mag[2048];
	double data_master_real[2048];
	double data_master_image[2048];

	int temptotalcount ;
	double error_rate_s22;
	// 원래 local function
	int nOrderCount1 ;

private:
	int createDirectoryRecursively( LPCTSTR path );
	

};

