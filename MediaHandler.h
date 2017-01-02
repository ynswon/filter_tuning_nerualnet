#pragma once
#define _STR_MACH_STATUS_BLANK					"BLANK"
#define _STR_MACH_STATUS_PROJECT_SETTING		"PROJECT_SETTING"
#define _STR_MACH_STATUS_COLLECTING_SAMPLE_DATA "COLLECTING_SAMPLE_DATA"
#define _STR_MACH_STATUS_MASTER_DATA			"MASTER_DATA"
#define _STR_MACH_STATUS_BUILD_FILTER_MODE		"BUILD_FILTER_MODE"
#define _STR_MACH_STATUS_TUNING_PROCESS			"TUNING_PROCESS"
#include <queue>
enum MACHINE_STATUS
{
	_MACH_STATUS_BLANK,
	_MACH_STATUS_PROJECT_SETTING,
	_MACH_STATUS_COLLECTING_SAMPLE_DATA,
	_MACH_STATUS_MASTER_DATA,
	_MACH_STATUS_BUILD_FILTER_MODE,
	_MACH_STATUS_TUNING_PROCESS,
};
struct SServiceQueueData
{
	CString order;
	CString feedback_msg_id;
};
typedef struct _SFE_WAVEFORM
{
	boolean updated[128];
	int count;
	double data[128][5][2048];
	int data_source; // 0: internal, 1: file, 2: external;
	CTime time; 
} SFE_WAVEFORM;

class CMediaHandler
{

public:
	CMediaHandler(void);
	~CMediaHandler(void);
public:
	void start(); 
	UINT static ThreadPredictionAndLearning(LPVOID _mothod);
	UINT static ThreadMsgInbound(LPVOID _mothod);
	UINT static ThreadServiceQueue(LPVOID _mothod);

	UINT static ThreadOnlyForFetch(LPVOID _mothod);
	UINT static ThreadOnlyForCalculation(LPVOID _mothod);

	UINT static fetch_waveform();
	UINT static process_waveform_after_fetch(bool _is_once_prediction = true);
	UINT static process_prediction();
	UINT static process_prediction_once();
	UINT static process_collect_sample_data(int _is_master = 0, int _is_save = 0);

	// 주요 데이터 송부
	int GenerateInfoXML(char* dest, int max_len);
	// 프로젝트 리스트 송부
	int GenerateInfoProjectListXML(char* dest, int max_len);

	// 프로젝트 상세 정보 송부 <info_project_property>
	int GenerateInfoProjectPropertyXML(char* xml_location, char* dest, int max_len, const char* feedback_msg = NULL);
	// 파형 데이터 생성
	int GenerateInputDataSetXML(const char* xml_location, char* dest, int max_len);
	int GenerateInputDataSetXML(const char* xml_location, char* dest, int max_len, int cnt, double (*mymy)[2048], int port_dest, int port_source);
	
	
	
	void LoadProject(CString project_name);
	// 프로젝트를 로드하고 ini업데이트 및 초기 세팅까지 완료
	void LoadProjectAndFilterTuning(CString project_name, CString filter_name, CString tuning_phase_name);
	void DoMeasurement();
	int LoadMasterWaveForm(int idx_pole = -1);
	// 0이면 정상, 1이면 문제 있는 거임
	int LoadWaveFormFromXML(tinyxml2::XMLDocument* doc, 
		boolean out_waveform_updated[128],
		int& waveform_count, 
		double (*out_waveform)[5][2048]); 

	int LoadWaveFormFromXML(tinyxml2::XMLDocument* doc, 
		SFE_WAVEFORM& data); 

	int LoadWaveFormFromXMLFile(CString location); 

	int DoMeasurementFT(int idx_pole, int degree, int product_type=1); // pole 번호 1~n
	int DoMeasurementRT(int idx_pole, int degree, int product_type=1); // pole 번호 1~n

	int DoMeasurementMasterFT(int idx_pole, int product_type=1); // pole 번호 1~n
	int DoMeasurementMasterRT(int idx_pole, int product_type=1); // pole 번호 1~n

	/// error_type 2가 손부사장님께서 직접 제시해 주신 유사도
	//  0~2는 mag로
	//  3~5는 phase로
	double GetErrorRate(
		int error_type,
		int idx_refl,
		int idx_trans,
		int nTotalPointRefl,
		double (*waveform_refl)[2048],
		int nTotalPointTran,
		double (*waveform_tran)[2048] );
	double GetFinalErrorRate(int error_type);
	
	// 160903에 xml로 정규화 시켜야 함
	void LearnFT_with_rprop();
	void LearnFT_with_rprop_HardCoding();

	// 160903에 xml로 정규화 시켜야 함
	void LearnRT_with_rprop(); 
	void LearnRT_with_rprop_HardCoding(); 

	void LearnRT_wo_rprop(); 

	// void PredictRT_wo_rprop(int nTotalPointsCharacter, double* character );

	// void PredictFT_wo_rprop(int nTotalPointsCharacter, double* character_array);
	void PredictRT_with_rprop(int nTargetPole, int nTotalPointsCharacter, double* character_array);
	// void PredictRT_with_rprop_HardCoding(int nTargetPole, int nTotalPointsCharacter, double* character_array);

	


	void PredictFTwithRProp(
		int ccount, 
		double* character_array);

	// refl_type 혹은 tran_type이
	// 0이면 real & image
	// 1이면 mag
	// 2이면 mag & phase
	// -1이면 사용하지 않음
	double* BuildCharacterWaveform(int& character_count,
		const int refl_type, const double (*waveform_refl)[2048], const int waveform_refl_count,
		const int tran_type, const double (*waveform_tran)[2048], const int waveform_tran_count,
		bool use_error_rate = false, double fErrorRate = 0.0,
		 double* master_array = NULL,
		double* augmented_array = NULL, const int augmented_array_count = 0, bool use_scaling = false);
	 
	
public:
	CString sDeviceName;
	BOOLEAN _is_internal;


	void CopyPresentFileDataTo(int& count, int idx, double (*mymy)[2048]);

	int IsRunning();

	void StartCollecting();
	void StartPrediction();
	void StartLearning();

	void StopCollecting();
	void StopPrediction();
	void StopLearning();

	void GoCyclePrediction();
	void SetIsRunning(int signal);
	void IncreaseFetchCount();
	void IncreasePingCount();
	int GetFetchCount();
	int GetPingCount();
	int GetDuration();
	int GetMsgCountFromServer();
	void PushServiceQueue(CString order, CString feedback_msg_id);
	int PopServiceQueue(CString& order, CString& feedback_msg_id);
	bool EmptyServiceQueue();

	void SetDataSourceMode(int mode); // 0: internal, 1: from file, 2: external
	void SetPredictionMode(int mode); // 0: c/c++, 1: rscript

	MACHINE_STATUS GetMachineStatus(CString& str);
	void GoToBlankStatus();
	void SetMachineStatus(MACHINE_STATUS next_status);

	int GetLearningTime();
public:
	double master_data[128][5][2048]; 
	int GetDataSourceMode();
	int GetPredictionMode();

	// 가장 마지막으로 predict에 사용한 points 수
	int nFinalTotalPoints;

	// 가장 마지막으로 predict에 사용한 refl idx
	int nFinalReflIdx; 
	CString sFianlRefl;

	// 가장 마지막으로 predict에 사용한 측정한 tran idx
	int nFinalTranIdx; 
	CString sFianlTran;

	double aFinalErrorRate[6];
	double final_waveform_refl[5][2048];
	double final_waveform_tran[5][2048];

	// 마지막으로 활용된 파형(prediction 이든, data samping 이든)
	// 왜냐하면 input_data_set의 통신 명령어가 
	// media program이 내부 계측기를 보고 있다 하더라도 유효하기 때문임
	SFE_WAVEFORM last_waveform;
	SFE_WAVEFORM last_waveform_for_auto;

	/*
	boolean last_updated[128];
	int last_count;
	double last_data[128][5][2048];
	*/

	void SetCollectSampleDataSignalAtThisTime(
		int idx_pole // 1~n
		, int degree
		, int product_type = 1// 1~ 
		);
	// idx_pole: 1~n
	void SetCollectMasterDataSignalAtThisTime(int idx_pole = -1);

	int GetLearningStopSignal();
private:
	// int mode_data_source;
	int mode_prediction_mode;

	// 계측기든 파일이든 외부든 마지막 데이터 저장소


	// 내부 계측기 읽어오는 경우
	/*
	boolean data_updated[128];
	int data_count;
	double data[128][5][2048];
	*/

	// xml 파일에서 읽어오는 경우
	SFE_WAVEFORM file_waveform;
	/*
	boolean data_from_xml_file_updated[128];
	int data_from_xml_count; 
	double data_from_xml_file[128][5][2048];
	*/

	// 외부 입력을 통해 읽어오는 경우

	SFE_WAVEFORM external_waveform;

	/*
	boolean data_from_external_updated[128];
	int data_from_external_count; 
	double data_from_external[128][5][2048];
	*/

	// Master 파형 데이터
	boolean master_data_updated[128];
	int master_data_count;
	int master_data_count_array[128][5];
	int start_time;
	int learning_start_time;


	 
	 
	void predict(
		int nTotalPointsCharacter,
		double* character_array);

	void predictByRScript();
	void UnsetCollectSampleDataSignal(); 
	int nFetchCount;
	int nPingCount;
	int nMsgCountFromServer;
	int exit_signal;
	
	int auto_fetch_signal;
	int auto_prediction_signal;

	int prediction_signal;
	int prediction_once_signal;
	int learning_signal;
	int learning_stop_signal;
	double learning_convergence_error_rate; // 수렴과 관련한 error rate
	//int collect_master_data_signal;
	int collect_sample_data_signal;
	int collect_sample_data_signal_at_this_time;
	int collect_sample_data_signal_at_this_time_master;
	int collect_sample_data_idx_pole;
	int collect_sample_data_degree;
	int collect_sample_data_product_type;
	int	_is_running;
	HANDLE prediction_handle;
	HANDLE msg_inbound_handle;
	HANDLE service_queue_handle;
	HANDLE only_calculation_handle;
	HANDLE only_fetch_handle;
	MACHINE_STATUS machine_status;

	std::queue<SServiceQueueData> qServiceQueue;

private:
	CRITICAL_SECTION cs_service_queue;
public:
	char* xml_waveform_output;
	static bool RankComp( const CString& const_lhs, const CString& const_rhs );

private:
	Rprop *rProp;
	char loaded_fileloc[1024];
};

