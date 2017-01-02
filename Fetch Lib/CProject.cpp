#include "StdAfx.h"
#include "NetworkAnalyzerCtr.h"




CProject::CProject(void)
{
	nTotalPoint = 0;
	nTotalPoles = 0;

	project_name = L"";
	filter_name = L"";
	sw_name = L"";

	project_data_directory = L"C:\\filter_tuning_program\\project_data\\";

	data_source_mode = 0;
}
int CProject::GetSweelRange()
{
	return nSweepRange;
}
int CProject::GetTotalPoles()
{
	return nTotalPoles;
}

int CProject::GetDataSourceMode()
{
	return data_source_mode;
}

void CProject::SetDataSourceMode( int mode )
{
	CString use_internal_str = L"0";
	CString use_file_str = L"0";
	CString use_external_str = L"0";

	if(mode==0) use_internal_str = L"1";
	if(mode==1) use_file_str = L"1";
	if(mode==2) use_external_str = L"1";
	
	WritePrivateProfileString(L"DEVICE",L"USE_INTERNAL",use_internal_str, GetProjectConfigINILoc());
	WritePrivateProfileString(L"DEVICE",L"USE_FILE",use_file_str, GetProjectConfigINILoc());
	WritePrivateProfileString(L"DEVICE",L"USE_EXTERNAL",use_external_str, GetProjectConfigINILoc());
	data_source_mode = mode;
}

void CProject::SetDataSourceInteralAddress( CString address )
{
	WritePrivateProfileString(L"NI488",L"OPTION",address, GetProjectConfigINILoc()); 
}
CString CProject::GetDataSourceInteralAddress()
{
	wchar_t input[128];
	GetPrivateProfileString(L"NI488",L"OPTION", L"", input, sizeof(input), GetProjectConfigINILoc());
	return CString(input);
}

int CProject::GetTotalPoints()
{
	return	nTotalPoint;
}
std::list<int> CProject::GetTraceOrder()
{
	return option_data.aTraceOrderInt;
}


void CProject::SetRTTargetPole( int target_pole )
{
	this->nTargetRTPole = target_pole;
}
int CProject::GetRTTargetPole()
{
	return nTargetRTPole;
}
 

std::list<CString> CProject::GetProjectNameList()
{
	CString tpath = GetProjectDataDirectory() + _T("*");
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath); //

	CString fileName;
	CString DirName;
	std::list<CString> mylist;



	while (bWorking)
	{
		//다음 파일 / 폴더 가 존재하면다면 TRUE 반환
		bWorking = finder.FindNextFile();
		//파일 일때
		if (finder.IsArchived())
		{
			//파일의 이름      
			//읽어온 파일 이름을 리스트박스에 넣음
		}
		// 디렉터리 일때
		if (finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName(); 
			if(_fileName.GetLength()==0 ||
				_fileName.GetLength()==1 && _fileName=="." ||
				_fileName.GetAt(0)=='.')
			{

			}
			else
			{ 
				mylist.push_back(_fileName);    

			}
		}
	}
	return mylist;
}

std::list<CString> CProject::GetFilterListByProject( CString project_name )
{
	CString tpath = GetProjectDataDirectory() + project_name + L"\\filter\\*";
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath); //

	CString fileName;
	CString DirName;
	std::list<CString> mylist;



	while (bWorking)
	{
		//다음 파일 / 폴더 가 존재하면다면 TRUE 반환
		bWorking = finder.FindNextFile();
		//파일 일때
		if (finder.IsArchived())
		{
			//파일의 이름      
			//읽어온 파일 이름을 리스트박스에 넣음
		}
		// 디렉터리 일때
		if (finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName(); 
			if(_fileName.GetLength()==0 ||
				_fileName.GetLength()==1 && _fileName=="." ||
				_fileName.GetAt(0)=='.')
			{

			}
			else
			{ 
				mylist.push_back(_fileName);    

			}
		}
	}
	return mylist;
}

std::list<CString> CProject::GetSWNameListByProjectAndFilter( CString project_name, CString filter_name )
{

	CString tpath = GetProjectDataDirectory() + project_name + L"\\filter\\" + filter_name + L"\\*";
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath); //

	CString fileName;
	CString DirName;
	std::list<CString> mylist;



	while (bWorking)
	{
		//다음 파일 / 폴더 가 존재하면다면 TRUE 반환
		bWorking = finder.FindNextFile();
		//파일 일때
		if (finder.IsArchived())
		{
			//파일의 이름      
			//읽어온 파일 이름을 리스트박스에 넣음
		}
		// 디렉터리 일때
		if (finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName(); 
			if(_fileName.GetLength()==0 ||
				_fileName.GetLength()==1 && _fileName=="." ||
				_fileName.GetAt(0)=='.')
			{

			}
			else
			{ 
				mylist.push_back(_fileName);    

			}
		}
	}
	return mylist;
}

bool CProject::HasMasterDataFromFile( CString project_name, CString filter_name, CString sw_name )
{
	CString file_full_path;
	file_full_path = 
		GetProjectDataDirectory() 
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\criteria_data\\master.xml";
	if(!PathFileExists(file_full_path))
	{
		return false;
	}

	return true;
}

bool CProject::HasWeightDataFromFile( CString project_name, CString filter_name, CString sw_name, int* require_data, int* exist_data )
{ 
	CString file_prefix;
	CString file_full_path;
	file_prefix = 
		GetProjectDataDirectory() 
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\weight_data\\";
	if(sw_name=="FT")
	{ 
		if(require_data!=NULL) *require_data = 1;
		file_full_path = file_prefix + L"weights.csv";
		if(!PathFileExists(file_full_path))
		{
			if(exist_data!=NULL) *exist_data = 0;
			return false;
		}
		if(exist_data!=NULL) *exist_data = 1;
		return true;
	}
	else if(sw_name=="RT")
	{
		// [POLE_DATA] 의 POLE_COUNT를 읽어야 함
		int nPresentTotalPoles = GetTotalPolesFromFile(project_name,filter_name,sw_name);
		int i;
		int exist_count = 0;
		for(i=1;i<=nPresentTotalPoles;i++)
		{
			file_full_path.Format(L"%sweight_%d.csv",file_prefix, i);
			if(PathFileExists(file_full_path ))
			{
				exist_count++;
			}
		}

		if(require_data!=NULL)	*require_data	= nPresentTotalPoles;
		if(exist_data!=NULL)	*exist_data		= exist_count;

		if(nPresentTotalPoles == exist_count)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	} 
}

bool CProject::HasSampleDataFromFile( CString project_name, CString filter_name, CString sw_name, int* require_data, int* exist_data)
{
	CString file_prefix; 
	CString file_full_path;
	file_prefix = 
		GetProjectDataDirectory() 
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\learning_data\\";
	if(sw_name=="FT" || sw_name=="RT")
	{
		// [POLE_DATA] 의 POLE_COUNT를 읽어야 함
		int nPresentTotalPoles = GetTotalPolesFromFile(project_name,filter_name,sw_name);
		int i;
		int exist_count = 0;
		for(i=1;i<=nPresentTotalPoles;i++)
		{
			file_full_path.Format(L"%swf_%d.xml",file_prefix, i);
			if(PathFileExists(file_full_path ))
			{
				exist_count++;
			}
		}

		if(require_data!=NULL)	*require_data	= nPresentTotalPoles;
		if(exist_data!=NULL)	*exist_data		= exist_count;

		if(nPresentTotalPoles == exist_count)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	} 
}

int CProject::GetTotalPolesFromFile( CString project_name, CString filter_name, CString sw_name )
{
	CString ini_file;
	ini_file = 
		GetProjectDataDirectory()
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\sweep.ini";
	int pp = GetPrivateProfileInt(L"POLE_DATA",L"POLE_COUNT",0,ini_file);
	return pp;
}

CString  CProject::GetTransmissionFromFile( CString project_name, CString filter_name, CString sw_name, int channel )
{
	CString ini_file;
	ini_file = 
		GetProjectDataDirectory()
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\sweep.ini";
	CString sAppName;
	wchar_t input[128];

	sAppName.Format(L"CH%d",channel);
	GetPrivateProfileString(sAppName, L"TRANSMISSION", L"", input, sizeof(input), ini_file);
	return CString(input);
}


CString CProject::GetReflectionFromFile( CString project_name, CString filter_name, CString sw_name, int channel )
{
	CString ini_file;
	ini_file = 
		GetProjectDataDirectory()
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name 
		+  L"\\" 
		+ sw_name 
		+ L"\\sweep.ini";
	CString sAppName;
	wchar_t input[128];

	sAppName.Format(L"CH%d",channel);
	GetPrivateProfileString(sAppName, L"REFLECTION", L"", input, sizeof(input), ini_file);
	CString ret =  CString(input);
	return ret;
}

void CProject::LoadFilterTuningProgram(CString filter_name, CString sw_name)
{
	option_data.aTraceOrderInt.clear();
	option_data.aTraceOrderString.clear();

	wchar_t input[1024];


	this->filter_name = filter_name;
	this->sw_name = sw_name; //FT, RT

	CString file_full_path;
	file_full_path = 
		GetProjectDataDirectory()
		+ project_name 
		+ L"\\filter\\" 
		+ filter_name
		+ L"\\" + sw_name 
		+ L"\\sweep.ini";
	OutputDebugString(L"CProject::LoadFilterTuningProgram\r\n");
	if(!PathFileExists(file_full_path))
	{
		CString dstr, dstr2;
		dstr.Format(L"%s,%s,%s\n(Project Name, Filter Name, Tuning Phase Name)  존재하지 않습니다\r\n",project_name,filter_name,sw_name);
		dstr2.Format(L"디렉터리: %s\r\n",GetProjectDataDirectory());
		
		OutputDebugString(L"CProject::LoadFilterTuningProgram\r\n");
		OutputDebugString(dstr); 
		OutputDebugString(dstr2); 
		UnsetProjectAndFilterInfo();
		return;
	}

		
	int i, j, ch;
	nTotalPoint = 0;
	nSweepRange = 0;
	//nMinFreq = INT64_MAX;
	nMinFreq = 9223372036854775807i64;
	//nMaxFreq = INT64_MIN;
	nMaxFreq = (-9223372036854775807i64 - 1);
	nTotalPoles = GetPrivateProfileInt(L"POLE_DATA",L"POLE_COUNT",0,file_full_path);
	for(i=1;i<=nTotalPoles;i++)
	{ 
		CString name;
		name.Format(L"POLE_%d_TYPE",i);
		GetPrivateProfileString(L"POLE_DATA", name, L"", input, sizeof(input), file_full_path);
		aPoleProperty[i] = CString(input).Trim(); 

		name.Format(L"POLE_%d_NAME",i);
		GetPrivateProfileString(L"POLE_DATA", name, L"", input, sizeof(input), file_full_path);
		aPoleName[i] = CString(input).Trim(); 
	}


	for(ch=1;ch<=1;ch++)
	{
		CString app_name;

		memset(option_data.aTraceOption,0,sizeof(option_data.aTraceOption));
		memset(option_data.aTraceOrderMapping,0,sizeof(option_data.aTraceOrderMapping));

		app_name.Format(L"CH%d",ch);

		CString trace_name;
		CString trace_scale_func_name;
		// 유니코드 -> mb로 오면서 WCHAR에서 CHAR로 바뀜
		WCHAR trace_scale_func_wchar[1024];

		CString pole_name;



		option_data.nReflectionIdx = -1;
		option_data.nTransmissionIdx = -1;

		option_data.nUseRangePred = GetPrivateProfileInt(app_name, L"USE_RANGE_PRED", 0, file_full_path);

		GetPrivateProfileString(app_name, L"REFLECTION", L"", input, sizeof(input), file_full_path);
		option_data.sReflection = CString(input).Trim(); 

		option_data.nReflectionLearningType = 
			GetPrivateProfileInt(app_name, L"REFLECTION_LEARNING_TYPE", -1,file_full_path);


		GetPrivateProfileString(app_name, L"TRANSMISSION", L"", input, sizeof(input), file_full_path);
		option_data.sTransmission = CString(input).Trim(); 

		option_data.nTransmissionLearningType = 
			GetPrivateProfileInt(app_name, L"TRANSMISSION_LEARNING_TYPE", -1,file_full_path);

		if(option_data.sReflection.GetLength()>=3)
		{
			option_data.nReflectionIdx = (_ttoi(option_data.sReflection.Mid(1,1)) - 1) * 16 + (_ttoi(option_data.sReflection.Mid(2,1)) - 1);
		}
		else 
		{
			option_data.nReflectionIdx = -1;
		}


		if(option_data.sTransmission.GetLength()>=3)
		{
			option_data.nTransmissionIdx = (_ttoi(option_data.sTransmission.Mid(1,1)) - 1) * 16 + (_ttoi(option_data.sTransmission.Mid(2,1)) - 1);
		}
		else 
		{
			option_data.nTransmissionIdx = -1;
		}

		for(i=0;i<TOTAL_PORT;i++)
		{
			for(j=0;j<TOTAL_PORT;j++)
			{
				int trace_exist;
				//CString trace_scale_func;

				trace_name.Format(L"TRACE_S%d%d",i+1,j+1);
				trace_exist = GetPrivateProfileInt(app_name,trace_name,0,file_full_path);

				option_data.aTraceOption[i][j] = trace_exist;
				// option_data.sScalingFunction[i][j] = trace_scale_func;

				if(trace_exist)
				{
					trace_exist = trace_exist;
				}
			}
		}
		// 폴란드 프로그램 존중
		int nOrderCount = 0;
		for(i=0;i<TOTAL_PORT;i++)
		{
			if(!option_data.aTraceOption[i][i]) continue;

			CString dstr;
			dstr.Format(L"S%d%d",i+1,i+1);
			option_data.aTraceOrderString.push_back(dstr);
			option_data.aTraceOrderInt.push_back(i*16 + i);

			nOrderCount++;
			option_data.aTraceOrderMapping[i][i] = nOrderCount;
		}

		for(i=0;i<TOTAL_PORT;i++)
		{
			for(j=0;j<TOTAL_PORT;j++)
			{
				if(!option_data.aTraceOption[i][j]) continue;
				if(i==j) continue;
				CString dstr;
				dstr.Format(L"S%d%d",i+1,j+1);
				option_data.aTraceOrderString.push_back(dstr);
				option_data.aTraceOrderInt.push_back(i*16 + j);

				nOrderCount++;
				option_data.aTraceOrderMapping[i][j] = nOrderCount;

			}
		}

		// 측정 범위
		for(i=1;i<=TOTAL_PRED_RANGE;i++)
		{
			item_data[i-1].nExist = 0;
		}
		for(i=1;i<=TOTAL_PRED_RANGE;i++)
		{

			app_name.Format(L"CH%d_RANGE%d",ch,i);
			int nExist;
			int nStartFreqInMHz;
			int nStopFreqInMHz;

			nExist = GetPrivateProfileInt(app_name,L"EXIST",0,file_full_path);
			pred_data[i-1].nExist = nExist;
			pred_data[i-1].nStartFreq = 0;
			pred_data[i-1].nStopFreq = 0;
			if(nExist==0) continue;

			SFilterPredItem present_pred_item;

			nStartFreqInMHz = GetPrivateProfileInt(app_name,L"START_FREQ",0,file_full_path);
			nStopFreqInMHz = GetPrivateProfileInt(app_name,L"STOP_FREQ",0,file_full_path); 

			present_pred_item.nExist = 1;
			present_pred_item.nStartFreq = (__int64)nStartFreqInMHz * 1000000;
			present_pred_item.nStopFreq = (__int64)nStopFreqInMHz * 1000000;

			pred_data[i-1] = present_pred_item;
			nSweepRange++;

			double fStartFreq = present_pred_item.nStartFreq;
			double fStopFreq = present_pred_item.nStopFreq; 

		}
		for(i=1;i<=TOTAL_SWEEP_RANGE;i++)
		{
			app_name.Format(L"CH%d_SWEEP%d",ch,i);
			int nExist;
			int nStartFreqInMHz;
			int nStopFreqInMHz;
			int nPoints;
			int nInputFilter;
			int nPower;

			nExist = GetPrivateProfileInt(app_name,L"EXIST",0,file_full_path);
			if(nExist==0) break;

			SFilterSweepItem present_sweep_item;

			nStartFreqInMHz = GetPrivateProfileInt(app_name,L"START_FREQ",0,file_full_path);
			nStopFreqInMHz = GetPrivateProfileInt(app_name,L"STOP_FREQ",0,file_full_path);
			nPoints = GetPrivateProfileInt(app_name,L"POINTS",0,file_full_path);
			nInputFilter = GetPrivateProfileInt(app_name,L"INPUT_FILTER",0,file_full_path);
			nPower = GetPrivateProfileInt(app_name,L"POWER",0,file_full_path);


			present_sweep_item.nExist = 1;
			present_sweep_item.nStartFreq = (__int64)nStartFreqInMHz * 1000000;
			present_sweep_item.nStopFreq = (__int64)nStopFreqInMHz * 1000000;
			present_sweep_item.nPoint = nPoints;
			present_sweep_item.nInputFilter = nInputFilter;
			present_sweep_item.nPower = nPower;

			item_data[i-1] = present_sweep_item;
			nSweepRange++;

			double fStartFreq = present_sweep_item.nStartFreq;
			double fStopFreq = present_sweep_item.nStopFreq;

			for(j=0;j<nPoints;j++)
			{
				data_freq_ver2[j + nTotalPoint] = fStartFreq + (fStopFreq - fStartFreq) / (double)nPoints * (double)j;
			}
			nTotalPoint += nPoints;

			nMinFreq = min(nMinFreq, nStartFreqInMHz);
			nMaxFreq = max(nMaxFreq, nStopFreqInMHz);
		}
	}
	CString dstr;
	dstr.Format(L"%s,%s,%s\n(Project Name, Filter Name, Tuning Phase Name) 성공적으로 로딩되었습니다\r\n",project_name,filter_name,sw_name);
	OutputDebugString(dstr);
}


bool CProject::LoadProjectAndFilterTuningProgramFromINI()
{ 
	wchar_t input[1024];
	GetPrivateProfileString(L"SYSTEM", L"project_name", L"", input, sizeof(input), GetPresentINILoc());
	project_name = (CString)input;
	GetPrivateProfileString(L"SYSTEM", L"filter_name ", L"", input, sizeof(input), GetPresentINILoc());
	filter_name = (CString)input;
	GetPrivateProfileString(L"SYSTEM", L"sw_name", L"", input, sizeof(input), GetPresentINILoc());
	sw_name = (CString)input;

	_CreateDirectory(GetProjectDataDirectory() + project_name + L"\\"+ L"filter" + L"\\");
	if(filter_name!=L"" && sw_name != L"")
	{
		_CreateDirectory(GetProjectDataDirectory() + project_name + L"\\filter\\" + filter_name +  L"\\" + sw_name);

	}

	this->project_name = project_name;
	if(filter_name!=L"" && sw_name!=L"")
	{
		project->LoadFilterTuningProgram(filter_name, sw_name);
	}
	return 0;
}

void CProject::LoadProjectConfigINI()
{
	int mode_use_internal = GetPrivateProfileInt(L"DEVICE",L"USE_INTERNAL",0,GetProjectConfigINILoc());
	int mode_use_file = GetPrivateProfileInt(L"DEVICE",L"USE_FILE",0,GetProjectConfigINILoc());
	int mode_use_external = GetPrivateProfileInt(L"DEVICE",L"USE_EXTERNAL",0,GetProjectConfigINILoc());
	if(mode_use_internal) data_source_mode = 0;
	else if(mode_use_file) data_source_mode = 1;
	else if(mode_use_external)data_source_mode = 2;
}

void CProject::WriteProjectToINI( CString project_name )
{
	WritePrivateProfileString(L"SYSTEM", L"project_name", project_name, GetPresentINILoc());
	WritePrivateProfileString(L"SYSTEM", L"filter_name",L"", GetPresentINILoc());
	WritePrivateProfileString(L"SYSTEM", L"sw_name",L"", GetPresentINILoc());
}
void CProject::WriteProjectAndFilterTuningProgramToINI( CString project_name, CString filter_name, CString tuning_phase_name )
{
	WritePrivateProfileString(L"SYSTEM", L"project_name", project_name, GetPresentINILoc());
	WritePrivateProfileString(L"SYSTEM", L"filter_name", filter_name, GetPresentINILoc());
	WritePrivateProfileString(L"SYSTEM", L"sw_name", tuning_phase_name, GetPresentINILoc());
}

CString CProject::GetFTLearningFileLoc(int idx_pole, int degree, int product_type)
{
	if(product_type<=0) return L"";

	CString dstr;
	if(degree==0)
	{
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d.xml",idx_pole);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_%d.xml",idx_pole,product_type);
		}
	}
	else if(degree>0)
	{
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d_P%d.xml",idx_pole, degree);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_P%d_%d.xml",idx_pole, degree,product_type);
		}
	}
	else
	{
		degree = degree * -1;
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d_M%d.xml",idx_pole, degree);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_M%d_%d.xml",idx_pole, degree,product_type);
		}
	}

	return GetFTDirectory() + dstr;

}
CString CProject::GetRTLearningFileLoc(int idx_pole, int degree, int product_type)
{
	CString dstr;
	if(degree==0)
	{
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d.xml",idx_pole);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_%d.xml",idx_pole,product_type);
		}
	}
	else if(degree>0)
	{
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d_P%d.xml",idx_pole,degree);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_P%d_%d.xml",idx_pole,degree,product_type);
		}
	}
	else
	{
		degree = degree * -1;
		if(product_type==1)
		{
			dstr.Format(L"learning_data\\wf_%d_M%d.xml",idx_pole,degree);
		}
		else
		{
			dstr.Format(L"learning_data\\wf_%d_M%d_%d.xml",idx_pole,degree,product_type);
		}
	}

	return GetRTDirectory() + dstr;

}

CString CProject::GetFTMasterFileLoc(int idx_pole, int type_idx)
{
	CString tstr;
	if(type_idx == 0)
	{
		if(idx_pole<0)
		{
			tstr.Format(L"criteria_data\\master.xml",idx_pole); 
		}
		else
		{
			tstr.Format(L"criteria_data\\master_%d.xml",idx_pole); 
		}
	}
	else
	{
		tstr.Format(L"learning_data\\master_%d.xml",idx_pole);
//		return GetFTDirectory() + L"learning_data\\master.xml";
	}
	return GetFTDirectory() + tstr;
}
CString CProject::GetRTMasterFileLoc(int idx_pole, int type_idx)
{
	CString tstr;
	if(type_idx==0)
	{
		tstr.Format(L"criteria_data\\master_%d.xml",idx_pole);
	}
	else
	{
		tstr.Format(L"learning_data\\master_%d.xml",idx_pole);
	}
	return GetRTDirectory() + tstr;
}
CString CProject::GetFTDirectory()
{
	return GetTuningDirectory() + L"FT\\";
}
CString CProject::GetRTDirectory()
{
	return GetTuningDirectory() + L"RT\\";
}

void CProject::SetProjectDataDirectory( CString new_project_data_directory )
{
	this->project_data_directory = new_project_data_directory;
}

CString CProject::GetProjectDataDirectory()
{
	return project_data_directory;
	//return L"C:\\filter_tuning_program\\project_data\\";
}
CString CProject::GetPresentINILoc()
{
	return GetProjectDataDirectory() + L"present.ini";
}

CString CProject::GetProjectConfigINILoc()
{
	return GetProjectDataDirectory() + project_name + L"\\config.ini";
}

CString CProject::GetTuningDirectory()
{
	return GetProjectDataDirectory() + project_name + L"\\filter\\" + filter_name + "\\";
}
CString CProject::GetWeightFTFileLoc()
{
	return GetFTDirectory() + L"weight_data\\weights.csv";
}
CString CProject::GetWeightRTFileLoc(int i)
{
	CString retv;
	retv.Format(L"%sweight_data\\weight_%d.csv",GetRTDirectory(),i);
	return retv;
}
int CProject::IsFTLearned()
{
	// true: learned
	/*

	CString tpath = GetFTDirectory();
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath); //

	CString fileName;
	CString DirName;
	std::list<CString> mylist;



	while (bWorking)
	{
		//다음 파일 / 폴더 가 존재하면다면 TRUE 반환
		bWorking = finder.FindNextFile();
		//파일 일때
		if (finder.IsArchived())
		{
			//파일의 이름      
			//읽어온 파일 이름을 리스트박스에 넣음
		}
		// 디렉터리 일때
		if (finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName(); 
			if(_fileName.GetLength()==0 ||
				_fileName.GetLength()==1 && _fileName=="." ||
				_fileName.GetAt(0)=='.')
			{

			}
			else
			{ 
				mylist.push_back(_fileName);    

			}
		}
	}
	return mylist;*/
	return 0;
}

int CProject::IsRTLearned()
{
	// true: learned

	return 0;
}

CString CProject::GetProjectName()
{
	return project_name;
}

CString CProject::GetFilterName()
{
	return filter_name;
}

CString CProject::GetSWName()
{
	return sw_name;
}

void CProject::UnsetProjectAndFilterInfo()
{
	project_name = L"";
	filter_name = L"";
	sw_name = L"";

}

CString CProject::GetPolePropertyAt( int idx )
{
	if(idx<=0 || idx>MAX_NO_POLES) return L"INVALID";
	return aPoleProperty[idx];
}

CString CProject::GetPoleNameAt( int idx )
{
	if(idx<=0 || idx>MAX_NO_POLES) return L"INVALID";
	return aPoleName[idx];
}
