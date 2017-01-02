#pragma once
/*
 * ���͸� ���õ� �Լ��� �������� \�� �ٿ���
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
	int GetTotalPoints(); // �� ���� ����Ʈ ���� 200~300��
	int GetSweelRange(); // �� ���� ���� �� 1~3��

	std::list<CString> GetProjectNameList();
	std::list<CString> GetFilterListByProject(CString project_name);
	std::list<CString> GetSWNameListByProjectAndFilter(CString project_name, CString filter_name);
	bool HasMasterDataFromFile(CString project_name, CString filter_name, CString sw_name);
	bool HasWeightDataFromFile(CString project_name, CString filter_name, CString sw_name, int* require_data = NULL, int* exist_data = NULL);
	// SAMPLE DATA = LEARNING�� Ȱ��Ǵ� ������
	bool HasSampleDataFromFile(CString project_name, CString filter_name, CString sw_name, int* require_data = NULL, int* exist_data = NULL);
	int GetTotalPolesFromFile(CString project_name, CString filter_name, CString sw_name);
	CString GetReflectionFromFile(CString project_name, CString filter_name, CString sw_name, int channel = 1);
	CString GetTransmissionFromFile(CString project_name, CString filter_name, CString sw_name, int channel = 1);

	// ������Ʈ �ε�
	void LoadFilterTuningProgram(CString filter_name, CString tuning_phase_name);
	bool LoadProjectAndFilterTuningProgramFromINI();
	void LoadProjectConfigINI();

	// ������Ʈ INI�� ������Ʈ �̸� ������Ʈ, ����, Ʃ��PHASE�� �ʱ�ȭ
	void WriteProjectToINI(CString project_name);
	// ������Ʈ INI�� ������Ʈ, ����, Ʃ��PHASE �ۼ�
	void WriteProjectAndFilterTuningProgramToINI(CString project_name, CString filter_name, CString tuning_phase_name);

	SFilterPredItem pred_data[TOTAL_PRED_RANGE];
	SFilterSweepItem item_data[TOTAL_SWEEP_RANGE];
	SFilterSweepOption option_data;

	std::list<int> GetTraceOrder(); 

	/// ���� ������ �̿� ����
	void SetUsingInternalDevice(bool _is_using_internal);
	// RT Ʃ�׽� ������ POLE ��ȣ
	void SetRTTargetPole(int target_pole);
	int GetRTTargetPole();
	bool IsUsingInternalDevice();


	void SetProjectDataDirectory(CString new_project_data_directory);
	CString GetProjectDataDirectory();
	CString GetPresentINILoc(); // present.ini ��ġ
	CString GetProjectConfigINILoc(); // config.ini ��ġ
	// Ʃ�� ���͸�
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

	// TB�� �н��Ǿ��ִ���
	int IsFTLearned();
	// RT�� �н��Ǿ��ִ���
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

	// ���� ����
	int nTotalPoles;
	CString aPoleProperty[MAX_NO_POLES + 1];
	CString aPoleName[MAX_NO_POLES + 1];

	// 1~nTotalPoles
	int nTargetRTPole;

	// vna ����
	bool _using_internal_device_flag;

	double data_freq_ver2[2048];
	int nTotalPoint;
	int nSweepRange;
	__int64 nMinFreq;
	__int64 nMaxFreq;
};