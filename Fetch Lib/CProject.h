#pragma once
/*
 * 디렉터리 관련된 함수는 마지막에 \를 붙여줌
 */
class CProject
{
public:
	CProject(void);
	
	int GetTotalPoles();
	
	int GetDataSourceMode();
	void SetDataSourceMode(int mode);
	void SetDataSourceInteralAddress(CString address);
	CString GetDataSourceInteralAddress();
	// vna
	int GetTotalPoints(); // 총 측정 포인트 개수 200~300개
	int GetSweelRange(); // 총 측정 구간 수 1~3개

	std::list<CString> GetProjectNameList();
	std::list<CString> GetFilterListByProject(CString project_name);
	std::list<CString> GetSWNameListByProjectAndFilter(CString project_name, CString filter_name);
	bool HasMasterDataFromFile(CString project_name, CString filter_name, CString sw_name);
	bool HasWeightDataFromFile(CString project_name, CString filter_name, CString sw_name, int* require_data = NULL, int* exist_data = NULL);
	// SAMPLE DATA = LEARNING에 활용되는 데이터
	bool HasSampleDataFromFile(CString project_name, CString filter_name, CString sw_name, int* require_data = NULL, int* exist_data = NULL);
	int GetTotalPolesFromFile(CString project_name, CString filter_name, CString sw_name);
	CString GetReflectionFromFile(CString project_name, CString filter_name, CString sw_name, int channel = 1);
	CString GetTransmissionFromFile(CString project_name, CString filter_name, CString sw_name, int channel = 1);

	// 프로젝트 로드
	void LoadFilterTuningProgram(CString filter_name, CString tuning_phase_name);
	bool LoadProjectAndFilterTuningProgramFromINI();
	void LoadProjectConfigINI();

	// 프로젝트 INI에 프로젝트 이름 업데이트, 필터, 튜닝PHASE는 초기화
	void WriteProjectToINI(CString project_name);
	// 프로젝트 INI에 프로젝트, 필터, 튜닝PHASE 작성
	void WriteProjectAndFilterTuningProgramToINI(CString project_name, CString filter_name, CString tuning_phase_name);

	SFilterPredItem pred_data[TOTAL_PRED_RANGE];
	SFilterSweepItem item_data[TOTAL_SWEEP_RANGE];
	SFilterSweepOption option_data;

	std::list<int> GetTraceOrder(); 

	/// 내부 개측기 이용 설정
	void SetUsingInternalDevice(bool _is_using_internal);
	// RT 튜닝시 설정할 POLE 번호
	void SetRTTargetPole(int target_pole);
	int GetRTTargetPole();
	bool IsUsingInternalDevice();


	void SetProjectDataDirectory(CString new_project_data_directory);
	CString GetProjectDataDirectory();
	CString GetPresentINILoc(); // present.ini 위치
	CString GetProjectConfigINILoc(); // config.ini 위치
	// 튜닝 디렉터리
	// ex: C:\filter_tuning_program\project_data\roswin_filter\filter\LOW\TB
	CString GetTuningDirectory();
	
	CString GetFTDirectory();
	CString GetRTDirectory();

	CString GetFTLearningFileLoc(int idx_pole, int degree, int product_type);
	CString GetRTLearningFileLoc(int idx_pole, int degree, int product_type);

	CString GetFTMasterFileLoc(int idx_pole, int type_idx = 0);
	CString GetRTMasterFileLoc(int idx_pole, int type_idx = 0);
	
	CString GetWeightFTFileLoc();
	CString GetWeightRTFileLoc(int i);

	// TB가 학습되어있는지
	int IsFTLearned();
	// RT가 학습되어있는지
	int IsRTLearned();
	// 

	CString GetProjectName();
	CString GetFilterName();
	CString GetSWName();

	void UnsetProjectAndFilterInfo();

public:
	CString GetPolePropertyAt(int idx);// idx from 0~
	CString GetPoleNameAt(int idx);// idx from 0~
private:
	int data_source_mode;

	CString project_data_directory;
	CString project_name;
	CString filter_name;
	CString sw_name;

	// 필터 구조
	int nTotalPoles;
	CString aPoleProperty[MAX_NO_POLES + 1];
	CString aPoleName[MAX_NO_POLES + 1];

	// 1~nTotalPoles
	int nTargetRTPole;

	// vna 관련
	bool _using_internal_device_flag;

	double data_freq_ver2[2048];
	int nTotalPoint;
	int nSweepRange;
	__int64 nMinFreq;
	__int64 nMaxFreq;
};