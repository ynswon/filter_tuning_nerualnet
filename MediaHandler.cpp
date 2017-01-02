#include "StdAfx.h"
#include "MediaHandler.h"
#include "./Tiny Xml Lib/tinyxml2.h"

CMediaHandler::CMediaHandler(void)
{
	nFetchCount = 0;
	nPingCount = 0; 
	_is_internal = true;
	sDeviceName	 = L"ROHDE&SCHWARZ";

	namedPipe = new CNamedPipe(L"sfe_pipe_data_to_server",L"sfe_pipe_data_to_client");
	namedPipe->start();

	exit_signal = 0;
	auto_fetch_signal = 0;
	auto_prediction_signal = 0;

	learning_signal = 0;
	learning_stop_signal = 0;

	_is_running = 0;

	SYSTEM_INFO systemInfo;
	GetSystemInfo( &systemInfo);
	DWORD cpu_count = systemInfo.dwNumberOfProcessors;

	mode_prediction_mode = 0;
	// mode_data_source = 0;


	machine_status = MACHINE_STATUS::_MACH_STATUS_BLANK;

	start_time = time(0);

	nFinalReflIdx = 0;
	nFinalTranIdx = 0;

	InitializeCriticalSection(&cs_service_queue);

	memset(aFinalErrorRate,0,sizeof(aFinalErrorRate));
}


CMediaHandler::~CMediaHandler(void)
{
	exit_signal = 1;
	DeleteCriticalSection(&cs_service_queue);

}

void CMediaHandler::start()
{

	prediction_handle = AfxBeginThread(ThreadPredictionAndLearning, this);
	msg_inbound_handle = AfxBeginThread(ThreadMsgInbound, this);
	service_queue_handle = AfxBeginThread(ThreadServiceQueue, this);
	only_calculation_handle = AfxBeginThread(ThreadOnlyForCalculation, this);
	only_fetch_handle = AfxBeginThread(ThreadOnlyForFetch, this);
}

int CMediaHandler::GenerateInfoXML(char* dest, int max_len)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* info = doc->InsertEndChild( doc->NewElement( "info" ) );
	tinyxml2::XMLElement* sub[21] = { 
		doc->NewElement( "project_name" ),			// 0
		doc->NewElement( "filter_name" ),			// 1
		doc->NewElement( "sw_name" ),				// 2
		doc->NewElement( "machine_status" ),		// 3
		doc->NewElement( "ping_count" ),			// 4
		doc->NewElement( "pole_of_interest" ), 		// 5
		doc->NewElement( "duration" ),				// 6
		doc->NewElement( "device" ),				// 7
		doc->NewElement( "message_id" ),			// 8
		doc->NewElement( "number_of_poles" ),		// 9
		doc->NewElement( "prediction_pole_data" ),	// 10
		doc->NewElement( "transmission" ),			// 11
		doc->NewElement( "reflection" ),			// 12
		doc->NewElement( "error_0" ),				// 13
		doc->NewElement( "error_1" ),				// 14
		doc->NewElement( "error_2" ),				// 15
		doc->NewElement( "error_3" ),				// 16
		doc->NewElement( "error_4" ),				// 17
		doc->NewElement( "error_5" ),				// 18
		doc->NewElement( "learning_convergence_error_rate" ),				// 19
		doc->NewElement( "learning_time" ), // 20
	};
	char buf_output[128];
	CString sstr;

	CopyCStringToChar(project->GetProjectName(), buf_output);
	sub[0]->SetText(buf_output);

	CopyCStringToChar(project->GetFilterName(), buf_output);
	sub[1]->SetText(buf_output);

	CopyCStringToChar(project->GetSWName(), buf_output);
	sub[2]->SetText(buf_output);

	handler->GetMachineStatus(sstr);
	CopyCStringToChar(sstr, buf_output);
	sub[3]->SetText(buf_output);

	sub[4]->SetText(handler->GetPingCount());
	sub[5]->SetText(project->GetRTTargetPole());
	
	sub[6]->SetText(handler->GetDuration());
	sub[6]->SetAttribute("unit","second");

	int i;
	
	tinyxml2::XMLElement* sub_device[2] = { 
		doc->NewElement( "connection_type" ), 
		doc->NewElement( "name" )
	};
	if(_is_internal)	sub_device[0]->SetText("internal");
	else				sub_device[0]->SetText("external");

	sub[7]->InsertEndChild(sub_device[0]);
	sub[7]->InsertEndChild(sub_device[1]);

	// doc->SaveFile("./myxml.xml",true);	

	char msgid[128];
	sprintf(msgid,"%d_%d",
		++general_msg_id_increment, 
		time(NULL));
	sub[8]->SetText(msgid);

	char pole_msg[16];
	int nPoles = project->GetTotalPoles();
	sprintf(pole_msg,"%d",nPoles);
	sub[9]->SetText(pole_msg);

	for(i=0;i<nPoles;i++)
	{
		char pred_data[32];
		tinyxml2::XMLElement* sub_pole;
		tinyxml2::XMLElement* sub_pole_data[4] = { 
			doc->NewElement( "index" ), 
			doc->NewElement( "name" ), 
			doc->NewElement( "property" ), 
			doc->NewElement( "delta" )
		};

		sub_pole = doc->NewElement( "name" );
		sub_pole->SetAttribute("index", i+1);

		sub_pole_data[0]->SetText(i+1);

		CopyCStringToChar(prediction_output_name[i],pred_data);
		sub_pole_data[1]->SetText(pred_data);

		CopyCStringToChar(prediction_output_property[i],pred_data);
		sub_pole_data[2]->SetText(pred_data);
		 
		sub_pole_data[3]->SetText(prediction_output_result[i]);

		sub_pole->InsertEndChild(sub_pole_data[0]);
		sub_pole->InsertEndChild(sub_pole_data[1]);
		sub_pole->InsertEndChild(sub_pole_data[2]);
		sub_pole->InsertEndChild(sub_pole_data[3]);
		sub[10]->InsertEndChild(sub_pole);


	}

	char wave_msg[128]; 
	CopyCStringToChar(handler->sFianlRefl,wave_msg);
	sub[11]->SetText(wave_msg);


	CopyCStringToChar(handler->sFianlTran,wave_msg);
	sub[12]->SetText(wave_msg);
	/*
	sub[13]->SetText(handler->aFinalErrorRate[0]);

	sub[14]->SetText(handler->aFinalErrorRate[1]);

	sub[15]->SetText(handler->aFinalErrorRate[2]);

	sub[16]->SetText(handler->aFinalErrorRate[3]);

	sub[17]->SetText(handler->aFinalErrorRate[4]);

	sub[18]->SetText(handler->aFinalErrorRate[5]);
	*/

	sub[13]->SetText(handler->aFinalErrorRate[2]);
	sub[14]->SetText(handler->aFinalErrorRate[2]);
	sub[15]->SetText(handler->aFinalErrorRate[2]);
	sub[16]->SetText(handler->aFinalErrorRate[2]);
	sub[17]->SetText(handler->aFinalErrorRate[2]);
	sub[18]->SetText(handler->aFinalErrorRate[2]);

	sub[19]->SetText(handler->learning_convergence_error_rate);

	sub[20]->SetText(handler->GetLearningTime());

	for(i=0;i<21;i++)
	{
		info->InsertEndChild( sub[i] );
	}
	std::stringstream ss;
	tinyxml2::XMLPrinter printer;
	doc->Print( &printer );
	ss << printer.CStr();



	int len = strlen(printer.CStr());
	if(len > max_len) len = max_len;
	memset(dest,0,sizeof(char)*max_len);
	memcpy(dest, printer.CStr(),len);
	delete doc;
	return len;
}
int CMediaHandler::GenerateInfoProjectListXML(char* dest, int max_len)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* info = doc->InsertEndChild( doc->NewElement( "info_project_list" ) );
	std::list<CString> mylist = project->GetProjectNameList();

	for(std::list<CString>::iterator itor = mylist.begin();
		itor != mylist.end();
		itor++)
	{
		tinyxml2::XMLElement* sub = doc->NewElement( "project" );
		info->InsertEndChild(sub);

		tinyxml2::XMLElement* sub2 = doc->NewElement( "name" );
		sub2->SetText("aaaA");
		sub->InsertEndChild(sub2);
		
		CString str = *itor;

		 
		wchar_t* wchar_str;     //첫번째 단계(CString to wchar_t*)를 위한 변수
		char*    char_str;      //char* 형의 변수
		int      char_str_len;  //char* 형 변수의 길이를 위한 변수

		//1. CString to wchar_t* conversion
		wchar_str = str.GetBuffer(str.GetLength());

		//2. wchar_t* to char* conversion
		//char* 형에 대한길이를 구함
		char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
		char_str = new char[char_str_len];  //메모리 할당
		//wchar_t* to char* conversion
		WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0,0);  
		sub2->SetText(char_str);


		delete char_str;
		//delete wchar_str;
		// sub2->SetText(str.GetBuffer(str.GetLength()));
	}
	doc->SaveFile("./myxml.xml",true);
	
	std::stringstream ss;
	tinyxml2::XMLPrinter printer;
	doc->Print( &printer );
	ss << printer.CStr();



	int len = strlen(printer.CStr());
	if(len > max_len) len = max_len;
	memset(dest,0,sizeof(char)*max_len);
	memcpy(dest, printer.CStr(),len);
	delete doc; 

	return len;
}


int CMediaHandler::GenerateInputDataSetXML(const char* xml_location, char* dest, int max_len, int cnt, double (*mymy)[2048], int port_dest, int port_source)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* info = doc->InsertEndChild( doc->NewElement( "input_data_set" ) );
	   
	char title[16];
	sprintf(title, "s%d%d", (port_dest)+1, (port_source)+1);

	tinyxml2::XMLElement* sub = doc->NewElement( title );
	info->InsertEndChild(sub);

	tinyxml2::XMLElement* sub2 = doc->NewElement( "count" );
	sub2->SetText(cnt);
	sub->InsertEndChild(sub2); 


	char output[1024];
	for(int j=0;j<cnt;j++)
	{
		tinyxml2::XMLElement* sub_title = doc->NewElement( "value" );
		sub_title->SetAttribute("index",j);

		sub->InsertEndChild(sub_title);


		tinyxml2::XMLElement* sub_value;

		sub_value = doc->NewElement( "freq" );
		sprintf(output,"%lf",mymy[IDX2_FREQ][j]);
		sub_value->SetText(output);
		sub_title->InsertEndChild(sub_value);

		sub_value = doc->NewElement( "real" );
		sprintf(output,"%lf",mymy[IDX2_REAL][j]);
		sub_value->SetText(output);
		sub_title->InsertEndChild(sub_value);

		sub_value = doc->NewElement( "image" );
		sprintf(output,"%lf",mymy[IDX2_IMAGE][j]);
		sub_value->SetText(output);
		sub_title->InsertEndChild(sub_value);

		sub_value = doc->NewElement( "mag" );
		sprintf(output,"%lf",mymy[IDX2_MAG][j]);
		sub_value->SetText(output);
		sub_title->InsertEndChild(sub_value);

		sub_value = doc->NewElement( "phase" );
		sprintf(output,"%lf",mymy[IDX2_PHASE][j]);
		sub_value->SetText(output);
		sub_title->InsertEndChild(sub_value);

	}

	if(xml_location!=NULL)
	{
		doc->SaveFile(xml_location,true);
	}

	std::stringstream ss;
	tinyxml2::XMLPrinter printer;
	doc->Print( &printer );
	ss << printer.CStr();


	if(dest!=NULL)
	{
		int len = strlen(printer.CStr());
		if(len > max_len) len = max_len;
		memset(dest,0,sizeof(char)*max_len);
		memcpy(dest, printer.CStr(),len);

		delete doc;
		return len;
	}
	delete doc;
	return 0;
}
int CMediaHandler::GenerateInputDataSetXML(const char* xml_location, char* dest, int max_len)
{

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* info = doc->InsertEndChild( doc->NewElement( "input_data_set" ) );
	
	for(int i=0;i<128;i++)
	{
		if(!last_waveform.updated[i]) continue;
		int cnt = last_waveform.count;

		char title[16];
		sprintf(title, "s%d%d", (i/16)+1, (i%16)+1);

		tinyxml2::XMLElement* sub = doc->NewElement( title );
		info->InsertEndChild(sub);

		tinyxml2::XMLElement* sub2 = doc->NewElement( "count" );
		sub2->SetText(cnt);
		sub->InsertEndChild(sub2); 


		char output[1024];
		for(int j=0;j<cnt;j++)
		{
			tinyxml2::XMLElement* sub_title = doc->NewElement( "value" );
			sub_title->SetAttribute("index",j);
			
			sub->InsertEndChild(sub_title);
			

			tinyxml2::XMLElement* sub_value;

			sub_value = doc->NewElement( "freq" );
			sprintf(output,"%lf",last_waveform.data[i][IDX2_FREQ][j]);
			sub_value->SetText(output);
			sub_title->InsertEndChild(sub_value);

			sub_value = doc->NewElement( "real" );
			sprintf(output,"%lf",last_waveform.data[i][IDX2_REAL][j]);
			sub_value->SetText(output);
			sub_title->InsertEndChild(sub_value);

			sub_value = doc->NewElement( "image" );
			sprintf(output,"%lf",last_waveform.data[i][IDX2_IMAGE][j]);
			sub_value->SetText(output);
			sub_title->InsertEndChild(sub_value);

			sub_value = doc->NewElement( "mag" );
			sprintf(output,"%lf",last_waveform.data[i][IDX2_MAG][j]);
			sub_value->SetText(output);
			sub_title->InsertEndChild(sub_value);

			sub_value = doc->NewElement( "phase" );
			sprintf(output,"%lf",last_waveform.data[i][IDX2_PHASE][j]);
			sub_value->SetText(output);
			sub_title->InsertEndChild(sub_value);

		}

		
	} 
	if(xml_location!=NULL)
	{
		doc->SaveFile(xml_location,true);
	}

	std::stringstream ss;
	tinyxml2::XMLPrinter printer;
	doc->Print( &printer );
	ss << printer.CStr();


	if(dest!=NULL)
	{
		int len = strlen(printer.CStr());
		if(len > max_len) len = max_len;
		memset(dest,0,sizeof(char)*max_len);
		memcpy(dest, printer.CStr(),len);
	
		delete doc;
		return len;
	}
	delete doc;
	return 0;
}

void CMediaHandler::LoadProject(CString project_name)
{
	project->WriteProjectToINI(project_name);
	project->LoadProjectAndFilterTuningProgramFromINI();
	SetMachineStatus(MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING);
	LoadMasterWaveForm();
	project->LoadProjectConfigINI();
	fetch_waveform();

}
void CMediaHandler::LoadProjectAndFilterTuning( CString project_name, CString filter_name, CString tuning_phase_name )
{

	project->WriteProjectAndFilterTuningProgramToINI(project_name,filter_name,tuning_phase_name);
	project->LoadProjectAndFilterTuningProgramFromINI();
	if(tuning_phase_name=="RT")
	{
		project->SetRTTargetPole(1);
	}
	SetMachineStatus(MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING);
	LoadMasterWaveForm();
	project->LoadProjectConfigINI();
	fetch_waveform();
}

void CMediaHandler::DoMeasurement()
{
	ctr->fetch_ver2(0,0,0,false);
}
void CMediaHandler::CopyPresentFileDataTo(int& count, int idx, double (*mymy)[2048])
{
	int i,itor_idx2;
	if(file_waveform.updated[idx]==false)
	{
		return;
	}
	count = this->file_waveform.count;
	for(itor_idx2=0;itor_idx2<5;itor_idx2++)
	{
		for(i=0;i<count;i++)
		{
			mymy[itor_idx2][i] = file_waveform.data[idx][itor_idx2][i];
		}
	}
}
int CMediaHandler::DoMeasurementFT(int idx_pole, int degree, int product_type)
{
	char xml_directory[1024];
	 
	CString fileloc = project->GetFTLearningFileLoc(idx_pole,degree,product_type);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(fileloc);
	if(bWorking)
	{
		// 기존 파일이 있으면.old에 백업
		MoveFile(fileloc, fileloc + L".old");
	}

	// Master 파형 저장
	CopyCStringToChar(fileloc, xml_directory);
	GenerateInputDataSetXML(xml_directory,NULL,0);
	return 0;
}
int CMediaHandler::DoMeasurementRT(int idx_pole, int degree, int product_type)
{

	char xml_directory[1024]; 
	//CString fileloc = project->GetRTMasterFileLoc(idx_pole);
	CString fileloc = project->GetRTLearningFileLoc(idx_pole,degree,product_type);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(fileloc);
	if(bWorking)
	{
		// 기존 파일이 있으면.old에 백업
		MoveFile(fileloc, fileloc + L".old");
	}

	// Master 파형 저장
	CopyCStringToChar(fileloc, xml_directory);
	GenerateInputDataSetXML(xml_directory,NULL,0);
	return 0;
}

double CMediaHandler::GetErrorRate( 
	int error_type,
	int idx_refl,
	int idx_trans, 
	int nTotalPointsRefl, 
	double (*waveform_refl)[2048], 
	int nTotalPointsTran,
	double (*waveform_tran)[2048] )
{

	int i;
	double ret = 0.0;
	double ret2 = 0.0;
	for(i=0;i<nTotalPointsRefl;i++)
	{
		if(waveform_refl==NULL)
		{
			AfxMessageBox(L"XXX");
		}
		else
		{
			if(error_type==0) ret += abs(master_data[idx_refl][IDX2_MAG][i] - waveform_refl[IDX2_MAG][i]);
			else if(error_type==1) ret += pow(master_data[idx_refl][IDX2_MAG][i] - waveform_refl[IDX2_MAG][i],2);
			else if(error_type==2)
			{
				ret += pow(master_data[idx_refl][IDX2_REAL][i]  - waveform_refl[IDX2_REAL][i], 2);
				ret2 += pow(master_data[idx_refl][IDX2_IMAGE][i] - waveform_refl[IDX2_IMAGE][i], 2);
			}
			else if(error_type==3) ret += abs(master_data[idx_refl][IDX2_PHASE][i] - waveform_refl[IDX2_PHASE][i]);
			else if(error_type==4) ret += pow(master_data[idx_refl][IDX2_PHASE][i] - waveform_refl[IDX2_PHASE][i],2);
			else if(error_type==5)
			{
				ret += pow(master_data[idx_refl][IDX2_PHASE][i]  - waveform_refl[IDX2_PHASE][i], 2);
			}
		}
	}


	for(i=0;i<nTotalPointsTran;i++)
	{
		if(waveform_refl==NULL && waveform_tran==NULL)
		{
			AfxMessageBox(L"XXXXXX");

		}
		else
		{
			/*
			if(error_type==2)
			{
//				ret += pow(master_data[idx_refl][IDX2_REAL][i]  - waveform_tran[IDX2_REAL][i], 2);
//				ret2 += pow(master_data[idx_refl][IDX2_IMAGE][i] - waveform_tran[IDX2_IMAGE][i], 2);
			}
			if(error_type==5)
			{
				ret += pow(master_data[idx_refl][IDX2_PHASE][i]  - waveform_tran[IDX2_PHASE][i], 2); 
			}
			*/

			if(error_type==0) ret += abs(master_data[idx_trans][IDX2_MAG][i] - waveform_tran[IDX2_MAG][i]);
			else if(error_type==1) ret += pow(master_data[idx_trans][IDX2_MAG][i] - waveform_tran[IDX2_MAG][i],2);
			else if(error_type==2)
			{
				ret += pow(master_data[idx_trans][IDX2_REAL][i]  - waveform_tran[IDX2_REAL][i], 2);
				ret2 += pow(master_data[idx_trans][IDX2_IMAGE][i] - waveform_tran[IDX2_IMAGE][i], 2);
			}
			else if(error_type==3) ret += abs(master_data[idx_trans][IDX2_PHASE][i] - waveform_tran[IDX2_PHASE][i]);
			else if(error_type==4) ret += pow(master_data[idx_trans][IDX2_PHASE][i] - waveform_tran[IDX2_PHASE][i],2);
			else if(error_type==5)
			{
				ret += pow(master_data[idx_trans][IDX2_PHASE][i]  - waveform_tran[IDX2_PHASE][i], 2);
			} 
		}
	}
	// mainly error_type 3 has been introduced


	if(error_type==2)
	{//sonhyunho
//		ret =  pow(ret, 0.5) + pow(ret2, 0.5);
		ret = ret / (double)nTotalPointsRefl;
	}
	if(error_type==5)
	{
		ret =  pow(ret, 0.5);
	}
	else
	{//sonhyunho
		ret =  pow(ret, 0.5) + pow(ret2, 0.5);
//		ret = ret / (double)nTotalPointsRefl;
	}
	return ret; 
}

double CMediaHandler::GetFinalErrorRate( int error_type )
{
	if(error_type<0 || error_type>=6) return 0.0;

	return aFinalErrorRate[error_type];
//	return GetErrorRate(error_type, nFinalTotalPoints, nFinalReflIdx, nFinalTranIdx, final_waveform_refl, final_waveform_tran); 
}
 
void CMediaHandler::LearnFT_with_rprop()
{
	// 각 폴별로 파일이 몇 개 있는지 빈도 수(0부터)
	int pole_file_count[MAX_NO_POLES] = {0, };
	double augmented_array[MAX_NO_POLES] = {0, };
	const double R_SCALE_MAX = 250;
	const double C_SCALE_MAX = 125;
	int waveform_loaded_count = 0; // 파일로부터 로딩된 파형 개수
	int waveform_mix_gen_count = 0; // 조합으로 생성된 파형 개수
	int i,j,k;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"FT") return;

	int nCountPointsCharacterMaster;
	int refl_idx = project->option_data.nReflectionIdx;
	int tran_idx = project->option_data.nTransmissionIdx;

	double er = handler->GetErrorRate(
		2, refl_idx, tran_idx, project->GetTotalPoints(),  master_data[refl_idx], project->GetTotalPoints(), master_data[tran_idx]);

	double* dest_master_character = handler->BuildCharacterWaveform(
		nCountPointsCharacterMaster,
		project->option_data.nReflectionLearningType, 
		master_data[refl_idx], 
		project->GetTotalPoints(),
		project->option_data.nTransmissionLearningType,  
		master_data[tran_idx], 
		project->GetTotalPoints(),
		false,
		er,
		false); 


	//
	CString basedir = project->GetFTDirectory() + L"learning_data\\"; // + project_name + L"\\filter\\*";
	CString tpath = basedir + L"\\*.xml";
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath);

	CString fileName;
	CString DirName;
	std::list<CString> mylist;


	std::map<int, double*> waveform_character;	
	std::list<CString> waveform_list;
	// 이름 불러오기
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
		if (!finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName();  
			if(_fileName.Right(4).MakeLower()!=L".xml") continue;
			if(_fileName.Left(3).MakeLower()!=L"wf_") continue;
			waveform_list.push_back(_fileName);
			
		}
	}
	waveform_list.sort(RankComp);
	int nCountPointsCharacter;
	for(std::list<CString>::iterator itor = waveform_list.begin();
		itor!= waveform_list.end();
		itor++)
	{
		LoadWaveFormFromXMLFile(project->GetFTDirectory() + L"learning_data\\" + *itor);
		CString _fileName = *itor; 
		_fileName = _fileName.Left(_fileName.GetLength() - 4);
		_fileName = _fileName.Right(_fileName.GetLength() -3); 
		_fileName = _fileName;
		CString temp1[10]; // 구해온 갯수만큼 동적 배열을 할당(CString배열)

		int cnt=0;
		CString strTok;
		while(AfxExtractSubString(strTok, _fileName, cnt, '_'))  // 문자를 잘라준다. (AfxExtractSubString = strTok)
			temp1[cnt++] = strTok;

		int index = _ttoi(temp1[0]);
		// double* augmented_array = new double[project->GetTotalPoles()];
		for(int i=0;i<project->GetTotalPoles();i++)
		{
			augmented_array[i] = 0.0;
		}
		int degree_pole;
		double degree_data=0;
		degree_pole = _ttoi(temp1[0]);
		if(temp1[1]!=L"")
		{
			if(temp1[1].Left(1)==L"M")
			{
				degree_data = pow(2, _ttof(temp1[1].Right( temp1[1].GetLength()-1 ))-1);
				degree_data *= -1;
			}
			else if(temp1[1].Left(1)==L"P")
			{
				degree_data = pow(2, _ttof(temp1[1].Right( temp1[1].GetLength()-1 ))-1);
			}
		}
		degree_pole--;
		if(degree_pole<0 || degree_pole>=project->GetTotalPoles())
		{
			OutputDebugString(L"파일이 올바르지 않습니다.");
			AfxMessageBox(L"파일이 올바르지 않습니다");
			ASSERT(0);
			continue;
		}
		augmented_array[degree_pole] = degree_data;
		pole_file_count[degree_pole]++;

		
		double er = handler->GetErrorRate(
			2, refl_idx, tran_idx, project->GetTotalPoints(),  file_waveform.data[refl_idx], project->GetTotalPoints(), file_waveform.data[index]);

		double* dest_character = handler->BuildCharacterWaveform(
			nCountPointsCharacter,
			project->option_data.nReflectionLearningType, 
			file_waveform.data[refl_idx], // IDX_DATA_S11~IDX_DATA_S88 참고 
			project->GetTotalPoints(),

			project->option_data.nTransmissionLearningType,  
			file_waveform.data[index], 
			project->GetTotalPoints(),
			false,
			er,
			NULL,
			augmented_array,
			project->GetTotalPoles(),
			false); 

		
		waveform_character[waveform_loaded_count] = dest_character;
			waveform_loaded_count++;
	}
	
	// xml 만들때 0껴서(waveform 정렬 필요함)

	// 셔플
	srand(time(0));
	int appearance[MAX_NO_POLES];
		
	
	//double appearance_degree[MAX_NO_POLES];
	double* appearance_degree = new double[MAX_NO_POLES];
	
	// 3배 정도 섞음
	int waveform_count = waveform_loaded_count;

	double** acc_character	   = new double*[waveform_loaded_count];

	waveform_mix_gen_count = waveform_loaded_count;

	//nCountPointsCharacter--;

	for(i=0;i<waveform_loaded_count*5 ;i++)
	{

		acc_character[i] = new double[nCountPointsCharacter]; // character 누적 배열		
	
		int scale = 4;
		int n_shuffle;

		n_shuffle = rand()% scale;
		//n_shuffle = rand() % project->GetTotalPoles();
		n_shuffle = rand() % (i/waveform_loaded_count+1)*scale ;

		n_shuffle = n_shuffle + 1;
		for(j=0;j<MAX_NO_POLES;j++)
		{
			appearance[j] = 0;
			appearance_degree[j] = 0.0;
		}
		for(j=0;j<nCountPointsCharacter;j++)
		{
			acc_character[i][j] = 0.0;
		}
		int acc_count = 0; // 실제로 누산한 카운트
		for(j=0;j<n_shuffle;j++)
		{
			int target_row;
			int apply_pole = -1;
			// row 선택 해
			target_row = rand() % waveform_count;
			// 선택된 row의 폴 위치를
			double* target_char = waveform_character[target_row];
			double target_char_degree;
			for(k=nCountPointsCharacter - project->GetTotalPoles();
				k<nCountPointsCharacter;
				k++)
			{
				if(target_char[k]!=0.0)
				{
					target_char_degree = target_char[k];
					if(target_char_degree<-10 || target_char_degree>10)
					{
						target_char_degree = target_char_degree;
					}
					apply_pole = k - nCountPointsCharacter + project->GetTotalPoles();
					break;
				}
			}
			// 이미 등록된 폴인가
			if(apply_pole>=0)
			{
				if(appearance[apply_pole]) continue;
				appearance[apply_pole] = 1;
				appearance_degree[apply_pole] = target_char_degree;
				for(k=0;k<nCountPointsCharacter;k++)
				{
					acc_character[i][k] += target_char[k];
				}
				acc_count++;
			}
		}
		for(k=0;k<nCountPointsCharacter - project->GetTotalPoles();k++)
		{
			acc_character[i][k] -= (double)( acc_count -1) * dest_master_character[k];
		}
		for(k=0; k<project->GetTotalPoles();k++)
		{
			// 왼쪽 k + nCountPointsCharacter - project->GetTotalPoles()
			// 530 19 기준으로 하면 530부터 증가함
			acc_character[i][k + nCountPointsCharacter - project->GetTotalPoles()] = appearance_degree[k]; 
		}
				
		 // 기존 waveform 뒤에 추가				
		waveform_character[  waveform_mix_gen_count ] = acc_character[i];
		waveform_mix_gen_count++;

				
	}

	for(k=nCountPointsCharacter - project->GetTotalPoles();k<nCountPointsCharacter;k++)
	{
		dest_master_character[k] =0 ;
	}
	waveform_character[  waveform_mix_gen_count] = dest_master_character;
	waveform_mix_gen_count++;
	
/*	for(int i=0; i<waveform_loaded_count; i++)
		delete[] acc_character[i];
	delete[] acc_character;
	*/
	
	
	// 테스트 용으로 csv로 저장해야함
	std:ofstream outFile("output_noscale.csv");
	for(int j=0; j< waveform_mix_gen_count; j++){
		double *temp = waveform_character[j];
		for(int i = 0 ; i < nCountPointsCharacter ; i++){
			outFile << temp[i]<< ",";
		}
		outFile << "\n";
		//	delete temp;
	}
/*	waveform_character = waveform_character;
	waveform_character[  waveform_mix_gen_count] = dest_master_character;
	waveform_mix_gen_count++;*/
	outFile.close();

	//scaling

	map<int, double*>::iterator iter;


	for(iter=waveform_character.begin(); iter!=waveform_character.end(); iter++)	{
		double *temp = iter->second;
		for(j=0; j<nCountPointsCharacter; j++)
		{
			if(j>=nCountPointsCharacter- project->GetTotalPoles())
			{				
				temp[j] = (temp[j] +100)/200;  //50 100 250 500
			}
			else
			{				
				temp[j] =  (temp[j] +2.5)/5;
			}

		}		
	}

	ofstream outFile2("output_withscale.csv");
	for(iter=waveform_character.begin(); iter!=waveform_character.end(); iter++)	{
		double *temp =  iter->second;
		for( i = 0 ; i < nCountPointsCharacter ; i++){
			outFile2 << temp[i]<< ",";
		}
		outFile2 << "\n";
		//	delete temp;
	}

	outFile2.close();
	// output에는 학습 개수도 필요함


	// TODO: Add your control notification handler code here	
	//seed random number generator
	int num_input	= nCountPointsCharacter-project->GetTotalPoles();
	int num_hidden	= 50;
	int num_output	= project->GetTotalPoles();
	int num_poles = project->GetTotalPoles();
	int num_train	= waveform_mix_gen_count;

	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 
/*
	double **test_input		= new double*[num_train+1];	
	double **target_output	= new double*[num_train+1];

	Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);

	for(int i = 0;i <= num_train;i++){
		test_input[i]		= new double[num_input];		
		target_output[i]	= new double[num_output];
	}

<<<<<<< .mine
	int index=0, lk;
||||||| .r89
	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output, num_train);
	//////////////////////////////////////////////////////////////////////////
	// rProp->inputfile("scaled_largedata.csv", num_train);
	////
	// 여길 대신 바꾸어야 함
	rProp->inputMap(waveform_character, waveform_loaded_count*4,nCountPointsCharacter- project->GetTotalPoles(), project->GetTotalPoles());
	rProp->Train();
	delete rProp;
=======
	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output, num_train);
	//////////////////////////////////////////////////////////////////////////
	// rProp->inputfile("scaled_largedata.csv", num_train);
	////
	// 여길 대신 바꾸어야 함
	rProp->inputMap(waveform_character, num_train ,nCountPointsCharacter- project->GetTotalPoles(), project->GetTotalPoles());
	rProp->Train(pole_file_count,  project->GetTotalPoles());
	
	handler->learning_convergence_error_rate = rProp->getErrorRate();
>>>>>>> .r111

<<<<<<< .mine
	for(iter=waveform_character.begin(); iter!=waveform_character.end(); iter++)	
	{
		double *temp =iter->second;
		for(lk=0; lk< nCountPointsCharacter- project->GetTotalPoles(); lk++)
		{
			test_input[index][lk] =		temp[lk];
||||||| .r89
=======
	if(handler->learning_stop_signal)
	{
		handler->learning_stop_signal = 0;		
>>>>>>> .r111

<<<<<<< .mine
		}
		for(lk=  nCountPointsCharacter- project->GetTotalPoles() ; lk<nCountPointsCharacter; lk++)
		{
			target_output[index][lk-( nCountPointsCharacter- project->GetTotalPoles())] =  temp[lk];
		}
		index++;
		delete temp;
	}

	FNN->Train(1500, num_train+1, learning_rate, test_input, target_output);	

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );		
	FNN->saveWeight(fileloc);

	for(int i = 0;i <= num_train;i++){
		delete[] test_input[i];
		delete[] target_output[i];
	}
	delete[] test_input;
	delete[] target_output;

	delete FNN;

	*/
	//////////////////////////////////////////////////////////////////////////
	// rProp->inputfile("scaled_largedata.csv", num_train);
	////
	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output, num_train);
	// 여길 대신 바꾸어야 함
	
	rProp->inputMap(waveform_character, num_train ,nCountPointsCharacter- project->GetTotalPoles(), project->GetTotalPoles());
	rProp->Train(pole_file_count,  project->GetTotalPoles());
	
	handler->learning_convergence_error_rate = rProp->getErrorRate();
	
	if(handler->learning_stop_signal)
	{
		handler->learning_stop_signal = 0;		

	}
	
	/*
	// 아래는 샘플 코드 이오니 업무에 참고하세요조
	while(true)
	{
		::Sleep(1000);
		if(handler->learning_stop_signal) 
		{
			//웨이트 중간 저장가능해야됨.
	
			OutputDebugString(L"학습 종료");
			handler->learning_stop_signal = 0;
			break;
		}
	}
	*/
	/*for(std::map<int, double*>::iterator itor = waveform_character.begin();
		itor !=waveform_character.end();
		itor++)
	{		
		delete itor->second;
	}
	waveform_character.clear();
	*/
	
	//delete FNN;
	delete rProp;

}

void CMediaHandler::LearnFT_with_rprop_HardCoding()
{

	
	int pole_file_count[MAX_NO_POLES] = {0, };
	double augmented_array[MAX_NO_POLES] = {0, };
	const double R_SCALE_MAX = 250;
	const double C_SCALE_MAX = 125;
	int waveform_loaded_count = 0; // 파일로부터 로딩된 파형 개수
	int waveform_mix_gen_count = 0; // 조합으로 생성된 파형 개수
	int i,j,k;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"FT") return;

	int nCountPointsCharacterMaster;

	int refl_idx = project->option_data.nReflectionIdx;
	int tran_idx = project->option_data.nTransmissionIdx;

	// TODO: Add your control notification handler code here	
	//seed random number generator
	int num_input	= 530;
	int num_hidden	= 50;
	int num_output	= project->GetTotalPoles();
	int num_poles = project->GetTotalPoles();
	int num_train	= 809;
	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 


	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output, num_train);
	rProp->inputfile("19_SCALED_TB_TRAIN_SUFFLE.csv", num_train);
	//rProp->inputfile_phase_mag("19_TB_TRAIN_SUFFLE.csv", num_train);
	rProp->Train();
	delete rProp;
}

void CMediaHandler::LearnRT_with_rprop()
{

	int pole_file_count[MAX_NO_POLES] = {0, };
	double augmented_array[MAX_NO_POLES] = {0, };
	const double R_SCALE_MAX = 100;
	const double C_SCALE_MAX = 50;
	int waveform_loaded_count = 0; // 파일로부터 로딩된 파형 개수
	int waveform_mix_gen_count = 0; // 조합으로 생성된 파형 개수
	int i,j,k;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"RT") return;

	int nCountPointsCharacterMaster;

	int refl_idx = project->option_data.nReflectionIdx;
	int tran_idx = project->option_data.nTransmissionIdx;

	
	for(int i=0;i<project->GetTotalPoles();i++)
	{
		augmented_array[i] = 0.0;
	}
	
	double er = handler->GetErrorRate(
		2, refl_idx, tran_idx, project->GetTotalPoints(),  master_data[refl_idx], project->GetTotalPoints(), master_data[tran_idx]);

	double* dest_master_character = handler->BuildCharacterWaveform(
		nCountPointsCharacterMaster,
		project->option_data.nReflectionLearningType, 
		master_data[refl_idx], 
		project->GetTotalPoints(),
		project->option_data.nTransmissionLearningType,  
		master_data[tran_idx], 
		project->GetTotalPoints(),
		true,
		er,
		NULL,
		augmented_array,
		project->GetTotalPoles(),
		false); 

	double *master_character = dest_master_character;		

	//
	CString basedir = project->GetRTDirectory() + L"learning_data\\"; // + project_name + L"\\filter\\*";
	CString tpath = basedir + L"\\*.xml";
	CFileFind finder;

	//CFileFind는 파일, 디렉터리가 존재하면 TRUE 를 반환함
	BOOL bWorking = finder.FindFile(tpath);

	CString fileName;
	CString DirName;
	std::list<CString> mylist;

	std::map<int, double*> waveform_character;
	std::list<CString> waveform_list;

	waveform_character[waveform_loaded_count] = dest_master_character;
	waveform_loaded_count++;
	// 이름 불러오기
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
		if (!finder.IsDirectory())
		{
			CString _fileName =  finder.GetFileName();  
			
			if(_fileName.Right(4).MakeLower()!=L".xml") continue;
			if(_fileName.Left(3).MakeLower()!=L"wf_") continue;
			waveform_list.push_back(_fileName);
			
		}
	}
	int nCountPointsCharacter;
	// std::sort(waveform_list.begin(), waveform_list.end(), RankComp);
	waveform_list.sort(RankComp);
	for(std::list<CString>::iterator itor = waveform_list.begin();
		itor!= waveform_list.end();
		itor++)	
	{
		LoadWaveFormFromXMLFile(project->GetRTDirectory() + L"learning_data\\" + *itor);
		CString _fileName = *itor; 
		_fileName = _fileName.Left(_fileName.GetLength() - 4);
		_fileName = _fileName.Right(_fileName.GetLength() -3); 
		_fileName = _fileName;
		CString temp1[10]; // 구해온 갯수만큼 동적 배열을 할당(CString배열)

		int cnt=0;
		CString strTok;
		while(AfxExtractSubString(strTok, _fileName, cnt, '_'))  // 문자를 잘라준다. (AfxExtractSubString = strTok)
			temp1[cnt++] = strTok;

		int index = _ttoi(temp1[0]);
		// double* augmented_array = new double[project->GetTotalPoles()];
		for(int i=0;i<project->GetTotalPoles();i++)
		{
			augmented_array[i] = 0.0;
		}
		int degree_pole;
		double degree_data=0;
		degree_pole = _ttoi(temp1[0]);
		if(temp1[1]!=L"")
		{
			if(temp1[1].Left(1)==L"M")
			{
				//degree_data = pow(2, _ttof(temp1[1].Right( temp1[1].GetLength()-1 ))-1);
				degree_data =  _ttof(temp1[1].Right( temp1[1].GetLength()-1 ));
				degree_data *= -1;
			}
			else if(temp1[1].Left(1)==L"P")
			{
				//degree_data = pow(2, _ttof(temp1[1].Right( temp1[1].GetLength()-1 ))-1);
				degree_data =  _ttof(temp1[1].Right( temp1[1].GetLength()-1 ));
			}
		}
		degree_pole--;
		if(degree_pole<0 || degree_pole>=project->GetTotalPoles())
		{
			OutputDebugString(L"파일이 올바르지 않습니다.");
			AfxMessageBox(L"파일이 올바르지 않습니다");
			ASSERT(0);
			continue;
		}
		augmented_array[degree_pole] = degree_data;
		pole_file_count[degree_pole]++;
		

		
		double er = handler->GetErrorRate(
			2, refl_idx, tran_idx, project->GetTotalPoints(),  file_waveform.data[refl_idx], project->GetTotalPoints(), file_waveform.data[index]);

		double* dest_character = handler->BuildCharacterWaveform(
			nCountPointsCharacter,
			project->option_data.nReflectionLearningType, 
			file_waveform.data[refl_idx], // IDX_DATA_S11~IDX_DATA_S88 참고 
			project->GetTotalPoints(),

			project->option_data.nTransmissionLearningType,  
			file_waveform.data[index], 
			project->GetTotalPoints(),
			false,
			er,
			master_character,
			augmented_array,
			project->GetTotalPoles(),
			false); 

	
		if(degree_data==0)
		{
			master_character = dest_character;

		}	

		waveform_character[waveform_loaded_count] = dest_character;
			waveform_loaded_count++;		
	}


	nCountPointsCharacter++; // error_rate added
	
	// 테스트 용으로 csv로 저장해야함
	std:ofstream outFile("output_noscale_rt.csv");
	for(int j=0; j< waveform_loaded_count; j++){
		double *temp = waveform_character[j];
		for(int i = 0 ; i < nCountPointsCharacter ; i++){
			outFile << temp[i]<< ",";
		}
		outFile << "\n";
		//	delete temp;
	}

	outFile.close();



	map<int, double*>::iterator iter;


	for(iter=waveform_character.begin(); iter!=waveform_character.end(); iter++)	{
		double *temp = iter->second;
		for(j=0; j<nCountPointsCharacter; j++)
		{
			if(j>=nCountPointsCharacter- project->GetTotalPoles())
			{				
				temp[j] = (temp[j] +10)/20;  //50 100 250 500
			}
			else if(j==nCountPointsCharacter- project->GetTotalPoles()-1)
			{
			//	temp[j] = (temp[j] +2.5)/5;  //50 100 250 500
			}
			else
			{				
				temp[j] =  (temp[j] +2.5)/5;
			}

		}		
	}

	ofstream outFile2("output_withscale_RT.csv");
	for(iter=waveform_character.begin(); iter!=waveform_character.end(); iter++)	{
		double *temp =  iter->second;
		for( i = 0 ; i < nCountPointsCharacter ; i++){
			outFile2 << temp[i]<< ",";
		}
		outFile2 << "\n";
		//	delete temp;
	}

	outFile2.close();


	// output에는 학습 개수도 필요함


	// TODO: Add your control notification handler code here	
	//seed random number generator
	int num_input	= nCountPointsCharacter-project->GetTotalPoles();
	int num_hidden	= 26;
	int num_output	= project->GetTotalPoles();

	int num_poles = project->GetTotalPoles();
	int num_train	= waveform_loaded_count;
	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 

	double **test_input		= new double*[num_train+1];
	double **input			= new double*[num_train+1];
	double **target_output	= new double*[num_train+1];


	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output, num_train);
	//////////////////////////////////////////////////////////////////////////
	// rProp->inputfile("scaled_largedata.csv", num_train);
	////
	// 여길 대신 바꾸어야 함
	rProp->inputMap(waveform_character, num_train ,num_input,  project->GetTotalPoles());
	rProp->Train_RT(pole_file_count,  project->GetTotalPoles());
	
	handler->learning_convergence_error_rate = rProp->getErrorRate();

	if(handler->learning_stop_signal)
	{
		handler->learning_stop_signal = 0;		

	}
	handler->learning_stop_signal = 0;
	/*
	// 아래는 샘플 코드 이오니 업무에 참고하세요조
	while(true)
	{
		::Sleep(1000);
		if(handler->learning_stop_signal) 
		{
			//웨이트 중간 저장가능해야됨.
	
			OutputDebugString(L"학습 종료");
			handler->learning_stop_signal = 0;
			break;
		}
	}
	*/
	for(std::map<int, double*>::iterator itor = waveform_character.begin();
		itor !=waveform_character.end();
		itor++)
	{
		delete[] itor->second;
	}
	delete rProp;



	/*Rprop *rProp  = new Rprop(num_input, num_hidden, num_output,num_train);

	rProp->inputfile("scaled_tw_master_exccept.csv" , num_train);
	rProp->Train_RT();

	delete rProp;*/
}

/*
void CMediaHandler::PredictRT_wo_rprop(int nTotalPointsCharacter, double* character )
{
	if(nTotalPointsCharacter<=0)
	{
		OutputDebugString(L"CMediaHandler::PredictFT 파형 데이터가 없습니다");
		return;
	}
	const double R_SCALE_MAX = 10;
	const double C_SCALE_MAX = 10;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"FT") return;

	int num_input	= nTotalPointsCharacter; // instead of 530 
	int num_hidden	= 26;
	int num_output	= project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 209;
	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];
	int i;
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 




	Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);
	//Rprop_Network *FNN = new Rprop_Network(num_input, num_hidden, num_output);

	// double charater[530];

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );
	FNN->loadWeights(fileloc);

	double *output = new double[num_output];

	FNN->Test(character, output);

	for(i=0;i<project->GetTotalPoles();i++)
	{
		prediction_output_result[i] = (output[i] *(maxs[i]-mins[i]) +mins[i])*10;	
	}


	double maxv[2] = {0.0, };
	int maxi[2] = {0, };

	for(i=0;i<project->GetTotalPoles();i++)
	{
		if((project->GetPolePropertyAt(i+1)==L"R" ||
			project->GetPolePropertyAt(i+1)==L"C") &&
			maxv[0]<=abs(prediction_output_result[i])
			)
		{
			maxv[0] = abs(prediction_output_result[i]);
			maxi[0] = i;
		}



		if(project->GetPolePropertyAt(i+1)==L"R" &&
			maxv[1]<=abs(prediction_output_result[i]))
		{

			if(maxv[1]<=abs(prediction_output_result[i]))
			{
				maxv[1] = abs(prediction_output_result[i]);
				maxi[1] = i;
			}
		}
		prediction_output_flag[i] = false;
	}
	prediction_output_flag[maxi[0]] = 1;
	prediction_output_flag[maxi[1]] = 1;


	delete[] output;
	delete FNN;
}
*/
/*
void CMediaHandler::PredictFT_wo_rprop(int nTotalPointsCharacter, double* character )
{
	if(nTotalPointsCharacter<=0)
	{
		OutputDebugString(L"CMediaHandler::PredictFT 파형 데이터가 없습니다");
		return;
	}
	const double R_SCALE_MAX = 250;
	const double C_SCALE_MAX = 125;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"FT") return;

	int num_input	= nTotalPointsCharacter; // instead of 530 
	int num_hidden	= 50;
	int num_output	= project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 1211;
	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];
	int i;
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 


	

	Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);
	//Rprop_Network *FNN = new Rprop_Network(num_input, num_hidden, num_output);

	// double charater[530];

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );
	FNN->loadWeights(fileloc);

	double *output = new double[num_output];
	 
	FNN->Test(character, output);

	for(i=0;i<project->GetTotalPoles();i++)
	{
		prediction_output_result[i] = (output[i] *(maxs[i]-mins[i]) +mins[i])*10;	
	}


	double maxv[2] = {0.0, };
	int maxi[2] = {0, };

	for(i=0;i<project->GetTotalPoles();i++)
	{
		if((project->GetPolePropertyAt(i+1)==L"R" ||
			project->GetPolePropertyAt(i+1)==L"C") &&
			maxv[0]<=abs(prediction_output_result[i])
		  )
		{
			maxv[0] = abs(prediction_output_result[i]);
			maxi[0] = i;
		}



		if(project->GetPolePropertyAt(i+1)==L"R" &&
			maxv[1]<=abs(prediction_output_result[i]))
		{

			if(maxv[1]<=abs(prediction_output_result[i]))
			{
				maxv[1] = abs(prediction_output_result[i]);
				maxi[1] = i;
			}
		}
		prediction_output_flag[i] = false;
	}
	prediction_output_flag[maxi[0]] = 1;
	prediction_output_flag[maxi[1]] = 1;
	 

	delete[] output;
	delete FNN;
}
*/

/*
void CMediaHandler::PredictRT_with_rprop_HardCoding( int nTargetPole, int nTotalPointsCharacter, double* character_array )
{
	// TODO: Add your control notification handler code here
	CString sstr, dstr;
	if(GetMachineStatus(sstr)!=MACHINE_STATUS::_MACH_STATUS_TUNING_PROCESS)
	{
		dstr.Format(L"CMediaHandler::PredictRT\r\n상태가 반드시 ::TUNING_PROCESS여야 합니다(현재 상태: %s)\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	// 요건이 맞는지 보기 + 파일이 있는지도 보기
	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()==L"") return;


	int index;
	int i, j;
	if(nTargetPole==-1) index = project->GetRTTargetPole();
	else index = nTargetPole;

	if(index<=0 || index>project->GetTotalPoles())
	{
		CString dstr;
		dstr.Format(L"Predict RT 실패, pole 번호가 맞지 않는 것 같습니다. %d(1~%d 사이의 값을 입력해주세요)\r\n", index,project->GetTotalPoles());
		OutputDebugString(dstr);
		return;
	}

	int num_input	= nTotalPointsCharacter+1;
	int num_hidden	= 26; //29
	int num_output	= project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 209;//308

	double learning_rate = 0.2;


	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES]; 
	// 뉴럴넷 연산은 0부터임을 기억할 것
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)	==L"R")
		{
			maxs[i] = 100.0;
			mins[i] = -100.0;
		}
		else if(project->GetPolePropertyAt(i+1)==L"C")
		{
			maxs[i] = 50.0;
			mins[i] = -50.0;
		}
		else
		{
			ASSERT(0);
			OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다");
		}
	}

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightRTFileLoc(index), fileloc );


	if(strcmp(loaded_fileloc,fileloc)==0)
	{
		// nothing to do
	}
	else
	{
		strcpy(loaded_fileloc,fileloc);
		//if(rProp!=NULL) delete rProp;
		rProp = new Rprop(num_input, num_hidden, num_output, num_train);




		double **input			= new double*[num_train];
		double **target_output	= new double*[num_train];

		Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);

		for(int i = 0;i < num_train;i++){	
			input[i]			= new double[num_input];
			target_output[i]	= new double[num_output];
		}

		// 160814 정규화 시켜야댐
		// 셔플된 1211개의 트레이닝 데이터를 스케일링(2.5배) 한 입력데이터
		fstream inputFile;
		inputFile.open("19_SCALED_TW_TRAIN_SUFFLE.csv", ios::in);
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
				if (line.length() > 2 ) FNN->processLine222(line, num_input, num_output, input[i], target_output[i]);

				i++;
				if(i>=num_train) break;
			}		
		}
		inputFile.close();

		int localee = (nTargetPole-1) * 10 + ( nTargetPole -1 );
		double t1=0, t2=0, sum=0;
		for(int j=0; j<num_input-1;j++)
		{
			t1 = (character_array[j]*5)-2.5;
			t2 = (input[localee][j]*5)-2.5;

			sum += (t1-t2)*(t1-t2);
		}
		character_array[num_input-1]= sum/(num_input-1);

		rProp->loadWeights(fileloc);

		for(int i = 0;i < num_train;i++){	
			delete[] input[i]			;
			delete[] target_output[i]	;
		}
		delete[] input;
		delete[] target_output;


		delete FNN;
		
		std:ofstream outFile("character_output.csv");
		for(int j=0; j< nTotalPointsCharacter+1; j++){

			//outFile << (character_array[j]-0.5)*360<< ",";		
			//outFile << (character_array[j]*5)-2.5<< ",";		
			outFile << (character_array[j])<< ",";		

		}	
		outFile.close();
		

	}
	//Rprop
		
	// TODO: Add your control notification handler code here
		
	rProp->loadWeights(fileloc);
	double *output = new double[num_output];
	rProp->ComputeOutputs(character_array, output);
	

	prediction_output_result[index-1] = (output[index-1] *(20) -10)*100;//+120;
	cout << output[index-1] << endl; 
	delete[] output;
	//delete rProp;
	
}
*/


void CMediaHandler::PredictRT_with_rprop( int nTargetPole, int nTotalPointsCharacter, double* character_array )
{
	// TODO: Add your control notification handler code here
	CString sstr, dstr;
	if(GetMachineStatus(sstr)!=MACHINE_STATUS::_MACH_STATUS_TUNING_PROCESS)
	{
		dstr.Format(L"CMediaHandler::PredictRT\r\n상태가 반드시 ::TUNING_PROCESS여야 합니다(현재 상태: %s)\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	// 요건이 맞는지 보기 + 파일이 있는지도 보기
	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()==L"") return;


	int index;
	int i, j;
	if(nTargetPole==-1) index = project->GetRTTargetPole();
	else index = nTargetPole;

	if(index<=0 || index>project->GetTotalPoles())
	{
		CString dstr;
		dstr.Format(L"Predict RT 실패, pole 번호가 맞지 않는 것 같습니다. %d(1~%d 사이의 값을 입력해주세요)\r\n", index,project->GetTotalPoles());
		OutputDebugString(dstr);
		return;
	}

	int num_input	= nTotalPointsCharacter;
	int num_hidden	= 26; //29
	int num_output	= 1;//project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 209;//308

	double learning_rate = 0.2;


	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES]; 
	// 뉴럴넷 연산은 0부터임을 기억할 것
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)	==L"R")
		{
			maxs[i] = 100.0;
			mins[i] = -100.0;
		}
		else if(project->GetPolePropertyAt(i+1)==L"C")
		{
			maxs[i] = 50.0;
			mins[i] = -50.0;
		}
		else
		{
			ASSERT(0);
			OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다");
		}
	}

	char fileloc[1024];
	CopyCStringToChar( project->GetWeightRTFileLoc(index), fileloc );


	if(strcmp(loaded_fileloc,fileloc)==0)
	{
		// nothing to do
	}
	else
	{
		strcpy(loaded_fileloc,fileloc);
		//if(rProp!=NULL) delete rProp;
		rProp = new Rprop(num_input, num_hidden, num_output, num_train);
		rProp->loadWeights(fileloc);


	}
	//Rprop
		
	// TODO: Add your control notification handler code here
	
	double *output = new double[num_output];
	rProp->ComputeOutputs(character_array, output);
	

	//prediction_output_result[index-1] = (output[index-1] *(20) -10)*100;//+120;
	//cout << output[index-1] << endl; 

	prediction_output_result[index-1] = (output[0] *(20) -10)*100;//+120;
	cout << output[0] << endl; 

	delete[] output;
	//delete rProp;
	
}

void CMediaHandler::LearnRT_with_rprop_HardCoding()
{

	const double R_SCALE_MAX = 100;
	const double C_SCALE_MAX = 50;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"RT") return;


	//seed random number generator
	int num_input	= 531;
	int num_hidden	= 26;
	int num_output	= 19; //project->GetTotalPoles();
	int num_poles = project->GetTotalPoles();
	int num_train	= 209;

	double learning_rate = 0.2;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 



	Rprop *rProp  = new Rprop(num_input, num_hidden, num_output,num_train);

//	rProp->inputfile_phase_mag("19_LOW_TW_1_S22.csv" , num_train);	
//	rProp->inputfile_phase_mag("19_LOW_TW_S22_PHASE_MAG.csv" , num_train);
//	rProp->inputfile_phase_mag("19_LOW_TW_S22_PHASE.csv" , num_train);
	rProp->inputfile("19_SCALED_TW_TRAIN_SUFFLE.csv" , num_train);
	rProp->Train_RT();

	delete rProp;
}

void CMediaHandler::LearnRT_wo_rprop()
{
	const double R_SCALE_MAX = 100;
	const double C_SCALE_MAX = 50;

	// 상태 보기
	int i, j;
	CString sstr, dstr;
	if(GetMachineStatus(sstr)!=MACHINE_STATUS::_MACH_STATUS_BUILD_FILTER_MODE)
	{
		dstr.Format(L"상태가 반드시 ::BUILD_FILTER_MODE여야 합니다(현재 상태: %s)\r\n",dstr);
		OutputDebugString(dstr);
		return;
	}

	// 요건이 맞는지 보기
	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()==L"") return;
	
	//seed random number generator
	int num_input	= 530; // constant
	int num_hidden	= 26; // constant
	int num_output	=  project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 209;
	double learning_rate = 0.2;

	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];
	maxs[0] = 0;
	// 뉴럴넷 연산은 0부터임을 기억할 것
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R") { maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C") { maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else
		{
			ASSERT(0);
			OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다");
		}
	}
	

	double **test_input		= new double*[num_train];
	double **input			= new double*[num_train];
	double **target_output	= new double*[num_train];

	Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);

	for(int i = 0;i < num_train;i++){
		test_input[i]		= new double[num_input];
		input[i]			= new double[num_input];
		target_output[i]	= new double[num_output];
	}

	// 160814 정규화 시켜야댐
	// 셔플된 1211개의 트레이닝 데이터를 스케일링(2.5배) 한 입력데이터
	fstream inputFile;
	inputFile.open("19_SCALED_TW_TRAIN_SUFFLE.csv", ios::in);

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
			if (line.length() > 2 ) FNN->processLine222(line, num_input, num_output, input[i], target_output[i]);
			i++;
			if(i>=num_train) break;
		}		
	}
	inputFile.close();

	// 160814 정규화 시켜야댐
	//int index[] = { 17, 17,17, 13,18, 16,17,13, 17,14, 17, 9, 17, 16, 17, 17, 17, 16, 13};
	int index[] = { 11, 11,11, 11,11, 11,11,11, 11,11, 11, 11, 11, 11, 11, 11, 11, 11, 11};
	
	int index_sum =0;
	double **input_part			= new double*[100];
	double **output_part		= new double*[100];
	double *output_ref			= new double[19];
	for(i=1;i<=19; i++)
	{
		int index_count = 0;
		 
		cout << index_sum+1 << " - " << index_sum + index[i-1]<< endl;
		for(j=index_sum; j< index_sum+index[i-1]; j++)
		{
			output_part[index_count]			= target_output[j];
			input_part[index_count++]			= input[j];

		}
		index_sum += index[i-1];

		FNN->Train(1500, index[i-1], learning_rate, input_part, output_part);	


		char fileloc[1024];
		CopyCStringToChar( project->GetWeightRTFileLoc(i), fileloc );		
		FNN->saveWeight(fileloc);

	}

	 

	for(int i = 0;i < num_train;i++){
		delete[] input[i];
		delete[] target_output[i];
	}
	delete[] input;
	delete[] target_output;

	delete FNN;

}
/*
void CMediaHandler::PredictRT(int target_pole, int nTotalPointsRefl, double (*waveform)[2048], int nTotalPointsTran, double (*waveform_tran)[2048])
{
	// TODO: Add your control notification handler code here
	CString sstr, dstr;
	if(GetMachineStatus(sstr)!=MACHINE_STATUS::_MACH_STATUS_TUNING_PROCESS)
	{
		dstr.Format(L"상태가 반드시 ::TUNING_PROCESS여야 합니다(현재 상태: %s)\r\n",dstr);
		OutputDebugString(dstr);
		return;
	}
	// 요건이 맞는지 보기 + 파일이 있는지도 보기
	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()==L"") return;

	
	int index;
	int i, j;
	if(target_pole==-1) index = project->GetRTTargetPole();
	else index = target_pole;

	if(index<=0 || index>project->GetTotalPoles())
	{
		CString dstr;
		dstr.Format(L"Predict RT 실패, pole 번호가 맞지 않는 것 같습니다. %d(1~%d 사이의 값을 입력해주세요)\r\n", index,project->GetTotalPoles());
		OutputDebugString(dstr);
		return;
	}

	int num_input	= 530;
	int num_hidden	= 26;
	int num_output	= 1;
	int num_poles	= project->GetTotalPoles();
	int num_train	= 300;
	double learning_rate = 0.2;


	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES]; 
	// 뉴럴넷 연산은 0부터임을 기억할 것
	for(i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")
		{
			maxs[i] = 100.0;
			mins[i] = -100.0;
		}
		else if(project->GetPolePropertyAt(i+1)==L"C")
		{
			maxs[i] = 50.0;
			mins[i] = -50.0;
		}
		else
		{
			ASSERT(0);
			OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다");
		}
	}


	if(1)
	{
		Rprop
			*rProp = new Rprop(num_input, num_hidden, num_output, num_train);
		// TODO: Add your control notification handler code here
		double charater[530];
		char fileloc[1024];
		CopyCStringToChar( project->GetWeightRTFileLoc(index), fileloc );

		rProp->loadWeights(fileloc);

		for(int i=0; i<530; i++)
		{	
			if(i%2==0) charater[i] = waveform[IDX2_REAL][i/2];
			else	   charater[i] = waveform[IDX2_IMAGE][i/2];

			charater[i] = (charater[i]+1) / 2; 
		}
		 double *output = new double[num_output];
		rProp->ComputeOutputs(charater, output);

		prediction_output_result[index-1] = (output[0] *(20) -10)*100;//+120;
		// cout << output << endl; 
		delete[] output;
		delete rProp;
	}
	else{		

		Feedforward_Neural_Network
			*FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);
		// TODO: Add your control notification handler code here
		double charater[530];
		char fileloc[1024];
		CopyCStringToChar( project->GetWeightRTFileLoc(index), fileloc );
		FNN->loadWeights(fileloc);

		double *output = new double[num_output];

		for(int i=0; i<530; i++)
		{	
			if(i%2==0) charater[i] = waveform[IDX2_REAL][i/2];
			else	   charater[i] = waveform[IDX2_IMAGE][i/2];

			charater[i] = (charater[i]+1) / 2;
			//charater[i] = (charater[i]+2.5) / 5;
		}

		FNN->Test(charater, output);

		//prediction_output_result[index-1] = (output[index-1] *(maxs[index-1]-mins[index-1]) +mins[index-1])*10;
		//prediction_output_result[index-1] = (output[index-1] *(20) -10)*10;
		prediction_output_result[index-1] = (output[0] *(20) -10)*100;
		cout << output[index-1] << endl;
		//prediction_output_result[0] /= 100.0;
		delete[] output;
	}
}
*/
int CMediaHandler::LoadMasterWaveForm(int idx_pole)
{
	if(project->GetSWName() =="RT")
	{
		if(idx_pole<1)
		{
			idx_pole = 1;
		}
		LoadWaveFormFromXMLFile(project->GetRTMasterFileLoc(idx_pole,0));
	}
	else if(project->GetSWName() == "FT")
	{
		LoadWaveFormFromXMLFile(project->GetFTMasterFileLoc(idx_pole));
	}
	else
	{

	}
	memcpy(master_data_updated, file_waveform.updated,sizeof(file_waveform.updated));
	master_data_count = file_waveform.count;
	// memcpy(master_data_count_array, data_from_xml_count_array,sizeof(master_data_count_array)); 
	memcpy(master_data, file_waveform.data,sizeof(master_data));
	return 0; 
}
int CMediaHandler::LoadWaveFormFromXML( 
	tinyxml2::XMLDocument* doc,
	SFE_WAVEFORM& my_waveform )
{
	return 
		LoadWaveFormFromXML(doc, 
			my_waveform.updated,
			my_waveform.count,
			my_waveform.data);
}

int CMediaHandler::LoadWaveFormFromXML( 
	tinyxml2::XMLDocument* doc,
	boolean out_waveform_updated[128],
	int& waveform_count, 
	double (*out_waveform)[5][2048] )
{
	waveform_count = 0;
	memset(out_waveform_updated,0,sizeof(bool) * 128);
	memset(out_waveform,0,sizeof(out_waveform));
	CString ppp;
	int max_count = 0;



	int data_from_xml_count_array[128][5] = {0, };
	tinyxml2::XMLElement* root = doc->FirstChildElement();
	int count = 0;
	for( tinyxml2::XMLNode* ele = doc->FirstChildElement( "input_data_set" )->FirstChild();
		ele;
		ele = ele->NextSibling() )
	{
		++count;
		CString dstr;
		tinyxml2::XMLElement* elesub = ele->FirstChildElement("count");
		const char* name = ((tinyxml2::XMLElement*)ele)->Value();// elesub = elesub;
		const char* txt2 = elesub->GetText();

		int source, dest;
		sscanf(name,"s%1d%1d",&dest,&source);
		int cnt;

		sscanf(txt2,"%d",&file_waveform.count);

		int present_idx = (dest-1) * 16 + (source-1);
		file_waveform.updated[present_idx] = true;

		elesub = ele->FirstChildElement("value");

		int ppp = 0;
		for( tinyxml2::XMLNode* eleele = elesub;
			eleele;
			eleele = eleele->NextSibling() )
		{
			int myindex;
			((tinyxml2::XMLElement*)eleele)->QueryIntAttribute( "index", &myindex);
			myindex = myindex;
			tinyxml2::XMLElement* elefreq = eleele->FirstChildElement("freq");
			tinyxml2::XMLElement* elereal = eleele->FirstChildElement("real");
			tinyxml2::XMLElement* eleimage = eleele->FirstChildElement("image");
			tinyxml2::XMLElement* elemag = eleele->FirstChildElement("mag");
			tinyxml2::XMLElement* elephase = eleele->FirstChildElement("phase"); 

			double present_data = 0;
			if(elefreq!=NULL)
			{
				sscanf(elefreq->FirstChild()->ToText()->Value(), "%lf",&present_data);
				out_waveform[present_idx][IDX2_FREQ][myindex] = present_data;
				data_from_xml_count_array[present_idx][IDX2_FREQ]++;
				out_waveform_updated[present_idx] = 1;
			}
			if(elereal!=NULL)
			{
				sscanf(elereal->FirstChild()->ToText()->Value(), "%lf",&present_data);
				out_waveform[present_idx][IDX2_REAL][myindex] = present_data;
				data_from_xml_count_array[present_idx][IDX2_REAL]++;
				out_waveform_updated[present_idx] = 1;
			}
			if(eleimage!=NULL)
			{
				sscanf(eleimage->FirstChild()->ToText()->Value(), "%lf",&present_data);
				out_waveform[present_idx][IDX2_IMAGE][myindex] = present_data;
				data_from_xml_count_array[present_idx][IDX2_IMAGE]++;
				out_waveform_updated[present_idx] = 1;
			} 
			if(elemag!=NULL)
			{
				sscanf(elemag->FirstChild()->ToText()->Value(), "%lf",&present_data);
				out_waveform[present_idx][IDX2_MAG][myindex] = present_data;
				data_from_xml_count_array[present_idx][IDX2_MAG]++;
				out_waveform_updated[present_idx] = 1;
			} 
			if(elephase!=NULL)
			{
				sscanf(elephase->FirstChild()->ToText()->Value(), "%lf",&present_data);
				out_waveform[present_idx][IDX2_PHASE][myindex] = present_data;
				data_from_xml_count_array[present_idx][IDX2_PHASE]++;
			}  
			if(max_count < myindex)
			{
				max_count = myindex;
			}
		}
		ppp = ppp;
	}
	if(max_count>0)
	{
		waveform_count = max_count+1;
	}
	return 0;
}

int CMediaHandler::LoadWaveFormFromXMLFile(CString location)
{

	tinyxml2::XMLDocument doc;
	char* bbb = new char[strlen(CT2A(location))+1];
	strcpy(bbb,CT2A(location));
	if(doc.LoadFile( bbb )!=tinyxml2::XML_SUCCESS)
	{
		delete bbb;
		return 1;
	} 
	delete bbb;
	return LoadWaveFormFromXML(&doc, file_waveform.updated, file_waveform.count, file_waveform.data);
}
UINT CMediaHandler::process_waveform_after_fetch(bool _is_once_prediction)
{
	SFE_WAVEFORM* target_waveform;
	if(_is_once_prediction)
	{
		target_waveform = &handler->last_waveform;
	}
	else
	{
		target_waveform = &handler->last_waveform_for_auto;
	}

	int refl_idx = handler->nFinalReflIdx = project->option_data.nReflectionIdx;
	handler->sFianlRefl = project->option_data.sReflection;

	int tran_idx = handler->nFinalTranIdx = project->option_data.nTransmissionIdx;
	handler->sFianlTran = project->option_data.sTransmission;


	// master 파형을 
	memcpy(prediction_input_waveform_master, handler->master_data[refl_idx],sizeof(prediction_input_waveform_master));

	int target, index = 0;
	int wave_form_points = 0;
	for(std::list<int>::iterator itor = project->option_data.aTraceOrderInt.begin();
		itor!= project->option_data.aTraceOrderInt.end();
		itor++)
	{
		target = *itor;

		sprintf(prediction_input_label[index],"S%d%d",(target / 16)+1, (target % 16)+1); 
		memcpy(
			prediction_input_waveform[index], 
			target_waveform->data[target],
			sizeof(prediction_input_waveform[index])
			);
		index++;
	}

	if(refl_idx>=0) memcpy(prediction_input_waveform_refl,target_waveform->data[refl_idx],sizeof(prediction_input_waveform_refl));
	if(tran_idx>=0) memcpy(prediction_input_waveform_tran,target_waveform->data[tran_idx],sizeof(prediction_input_waveform_tran));

	return ANDY_YES;
}
UINT CMediaHandler::fetch_waveform()
{
	if(handler->GetDataSourceMode()==0) 
	{
		int retv = ctr->fetch_ver2(0,0,0,true);
		if(retv == ANDY_NO) return ANDY_NO;
		
		memset(handler->last_waveform.updated,0,sizeof(handler->last_waveform.updated));
		memset(handler->last_waveform.data,0,sizeof(handler->last_waveform.data));
		handler->last_waveform.count = project->GetTotalPoints();
		int i,j,k;
		for(i=0;i<128;i++)
		{
			if(ctr->GetIndivGetDataFetchSqc(i) != ctr->GetDataFetchSqc()) continue;
			handler->last_waveform.updated[i] = 1;
			for(j=0;j<5;j++)
			{
				for(k=0;k<handler->last_waveform.count;k++)
				{
					handler->last_waveform.data[i][j][k] = ctr->GetData(i,j,k);
 				}
			}
		}
	}
	else if(handler->GetDataSourceMode()==1) // from file
	{
		// present_xml_input는 stdafx에 정의되어 있음
		present_xml_input = L"./sample_data/160727/pole17_wonjun_remote_step_3.xml";
		handler->LoadWaveFormFromXMLFile(present_xml_input);

		for(int i=0;i<128;i++)
		{
			handler->CopyPresentFileDataTo(
				handler->last_waveform.count,
				i,
				handler->last_waveform.data[i]);
		}
	}
	else if(handler->GetDataSourceMode()==2) // from external
	{
		// memset(handler->last_waveform.updated,0,sizeof(handler->last_waveform.updated));
		handler->last_waveform = handler->external_waveform;

	}
	else
	{
		ASSERT(0);
		AfxMessageBox(L"정의되어 있지 않음");
	}
	return ANDY_YES;
}
UINT CMediaHandler::process_collect_sample_data(int _is_master, int _is_save)
{
	//
	// 1단계 
	// 계측기에서 데이터 불러오기
	//
	
	// handler->last_data 파형에 데이터 저장
	fetch_waveform();




	int refl_idx = handler->nFinalReflIdx = project->option_data.nReflectionIdx;
	handler->sFianlRefl = project->option_data.sReflection;

	int tran_idx = handler->nFinalTranIdx = project->option_data.nTransmissionIdx;
	handler->sFianlTran = project->option_data.sTransmission;

	int total_points = handler->nFinalTotalPoints = project->GetTotalPoints();


	if(refl_idx>=0)
	{
		memcpy(prediction_input_waveform_refl, handler->last_waveform.data[refl_idx], sizeof(prediction_input_waveform_refl));
	} 
	if(tran_idx>=0)
	{
		memcpy(prediction_input_waveform_tran, handler->last_waveform.data[tran_idx], sizeof(prediction_input_waveform_tran));
	}

	prediction_input_count = total_points;
	int retv = process_waveform_after_fetch();

	handler->aFinalErrorRate[0] = handler->GetErrorRate(0, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);
	handler->aFinalErrorRate[1] = handler->GetErrorRate(1, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);
	handler->aFinalErrorRate[2] = handler->GetErrorRate(2, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);
	handler->aFinalErrorRate[3] = handler->GetErrorRate(3, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);
	handler->aFinalErrorRate[4] = handler->GetErrorRate(4, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);
	handler->aFinalErrorRate[5] = handler->GetErrorRate(5, 
		refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);


	if(_is_save)
	{

		if(project->GetSWName()==L"RT")
		{
			if(_is_master)
			{
				handler->DoMeasurementMasterRT(
					handler->collect_sample_data_idx_pole);
				handler->LoadMasterWaveForm(handler->collect_sample_data_idx_pole);
			}
			else
			{
				handler->DoMeasurementRT(
					handler->collect_sample_data_idx_pole,
					handler->collect_sample_data_degree);
			}
		}
		else if(project->GetSWName()==L"FT")
		{
			if(_is_master)
			{
				handler->DoMeasurementMasterFT(
					handler->collect_sample_data_idx_pole);
				handler->LoadMasterWaveForm(handler->collect_sample_data_idx_pole);
			}
			else
			{
				handler->DoMeasurementFT(
					handler->collect_sample_data_idx_pole,
					handler->collect_sample_data_degree,
					handler->collect_sample_data_product_type);


			} 
		}

		handler->DoMeasurementFT(
			handler->collect_sample_data_idx_pole,
			handler->collect_sample_data_degree);

		if(project->GetSWName()==L"RT")
		{
			if(_is_master)
			{
				handler->DoMeasurementMasterRT(
					handler->collect_sample_data_idx_pole);
				handler->LoadMasterWaveForm(handler->collect_sample_data_idx_pole);
			}
			else
			{
				handler->DoMeasurementRT(
					handler->collect_sample_data_idx_pole,
					handler->collect_sample_data_degree);
			}
		}
		else if(project->GetSWName()==L"FT")
		{
			if(_is_master)
			{
				handler->DoMeasurementMasterFT(
					handler->collect_sample_data_idx_pole);
				handler->LoadMasterWaveForm();
			}
			else
			{
				handler->DoMeasurementFT(
					handler->collect_sample_data_idx_pole,
					handler->collect_sample_data_degree,
					handler->collect_sample_data_product_type);


			} 
		}
	}
	return 0;
}
UINT CMediaHandler::process_prediction_once()
{
	int i, j, k;
	double waveform[5][2048];

	// 프로젝트 정보 복사
	int total_points = handler->nFinalTotalPoints = project->GetTotalPoints();

	int refl_idx = handler->nFinalReflIdx = project->option_data.nReflectionIdx;
	handler->sFianlRefl = project->option_data.sReflection;

	int tran_idx = handler->nFinalTranIdx = project->option_data.nTransmissionIdx;
	handler->sFianlTran = project->option_data.sTransmission;

	//
	// 1단계 
	// 계측기에서 데이터 불러오기
	//
	int retv;
	retv = fetch_waveform();
	if(retv == ANDY_NO)
	{
		OutputDebugString(L"CMediaHandler::process_prediction Failed to Fetch Waveform");
		return ANDY_NO;
	}
	//
	// 2단계 
	// 데이터 중 필요한 것만 쓸어오기
	//
	prediction_input_count = total_points;
	retv = process_waveform_after_fetch(true);

	
	// handler->CopyPresentFileDataTo(total_points,tran_idx,prediction_input_waveform_tran);

	//
	// 3단계 
	// building character waveform, refl, tran 모두 고려해야 함
	//  
	int nCountPointsCharacter = 0;


	double er = handler->GetErrorRate(
		2, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);


	double* dest_character = handler->BuildCharacterWaveform(
		nCountPointsCharacter,
		project->option_data.nReflectionLearningType, 
		prediction_input_waveform_refl, 
		total_points,

		project->option_data.nTransmissionLearningType,  
		prediction_input_waveform_tran, 
		total_points,
		true,
		er,
		NULL,
		NULL, 0, true); 


	//
	// 4단계 
	// 예측단계
	//
	if(handler->GetPredictionMode()==0) // predict by c/c++ lib
	{
		memcpy(handler->final_waveform_refl, prediction_input_waveform_refl, sizeof(prediction_input_waveform_refl));
		memcpy(handler->final_waveform_tran, prediction_input_waveform_tran, sizeof(prediction_input_waveform_tran));
		
		if(total_points>0)
		{
			/*
			handler->predict(
				total_points,
				prediction_input_waveform_refl, 
				total_points,
				prediction_input_waveform_tran);
			*/
			handler->predict(
				nCountPointsCharacter, 
				dest_character);
			handler->IncreaseFetchCount();
		}
	}
	else if(handler->GetPredictionMode()==1) // predict by r script
	{
		handler->predictByRScript();
	}
	else
	{
		AfxMessageBox(L"이럴수없어");
	}

	//
	// 5단계 
	// 데이터 출력
	//
	handler->GenerateInputDataSetXML(NULL, handler->xml_waveform_output, 1024*1024);
	namedPipe->PushQueueOutbound(CString(handler->xml_waveform_output)); 
	handler->aFinalErrorRate[0] = handler->GetErrorRate(
		0, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);

	handler->aFinalErrorRate[1] = handler->GetErrorRate(
		1, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);

	handler->aFinalErrorRate[2] = handler->GetErrorRate(
		2, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);

	handler->aFinalErrorRate[3] = handler->GetErrorRate(
		3, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);

	handler->aFinalErrorRate[4] = handler->GetErrorRate(
		4, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);

	handler->aFinalErrorRate[5] = handler->GetErrorRate(
		5, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
		);


	// 정리
	delete dest_character;
}
UINT CMediaHandler::process_prediction()
{
	if(handler->prediction_once_signal)
	{
		process_prediction_once();
	}
	else
	{
		StartCounter();
		handler->last_waveform_for_auto = handler->last_waveform; 

		handler->auto_fetch_signal = 1;
		handler->auto_prediction_signal = 1;
		while(1)
		{
			::Sleep(1);
			if(handler->auto_fetch_signal!=0) continue;
			if(handler->auto_prediction_signal!=0) continue;
			break;
		}


		double tt = GetCounter();
		tt = tt;
		tt = tt * 1.0;
		CString dstr;
		dstr.Format(L"Time to calculate %.2lf \r\n",tt);
	}

	return 0;
}
UINT CMediaHandler::ThreadPredictionAndLearning(LPVOID _mothod)
{
	::Sleep(100);

	while(handler==NULL)
	{
		::Sleep(1);
	}
	handler->xml_waveform_output = new char[1024*1024];
	SetThreadAffinityMask( NULL, 1 << 3 );  
	

	handler->prediction_signal = 0;
	handler->prediction_once_signal = 0;
	handler->collect_sample_data_signal = 0;
	handler->collect_sample_data_signal_at_this_time = 0;
	handler->collect_sample_data_signal_at_this_time_master = 0;
	handler->learning_start_time = time(0);

	while(true)
	{  
		::Sleep(1);
		handler->SetIsRunning(0);

		if(handler->prediction_signal == 0 && 
			handler->prediction_once_signal == 0 &&
			handler->collect_sample_data_signal==0  &&
			handler->learning_signal==0)	continue;
		

		if(handler->prediction_signal || 
			handler->prediction_once_signal ||
			handler->learning_signal)
		{ 
			if(handler->learning_signal)
			{
				handler->SetMachineStatus(_MACH_STATUS_BUILD_FILTER_MODE); 

				if(project->GetSWName()==L"FT")
				{
					handler->LearnFT_with_rprop();
				}
				else if(project->GetSWName()==L"RT")
				{
					handler->LearnRT_with_rprop();
				}
				else
				{
					OutputDebugString(L"CMediaHandler::ThreadPredictionAndLearning FT, RT만 학습됨");
				}
				handler->learning_stop_signal = 0;
				handler->learning_signal = 0;
			}
			else
			{
				handler->SetMachineStatus(_MACH_STATUS_TUNING_PROCESS); 
				handler->SetIsRunning(1);


				// 0단계 계측기 데이터 초기 세팅

				ctr->initialize_model_builder();


	
				if(project->GetSWName()==L"FT")
				{
					handler->LoadMasterWaveForm(0);
				}
				while(true)
				{
					if(handler->prediction_signal == 0 && handler->prediction_once_signal == 0)	break;
					process_prediction();
					if(handler->prediction_once_signal) handler->prediction_once_signal = 0;
					::Sleep(1);
				}
				
			}
			::Sleep(1);
		}
		else if(handler->collect_sample_data_signal)
		{
			handler->SetMachineStatus(_MACH_STATUS_COLLECTING_SAMPLE_DATA);
			handler->SetIsRunning(1);

			while(1)
			{
				if(handler->collect_sample_data_signal==0) break;

				if(handler->collect_sample_data_signal_at_this_time)
				{
					process_collect_sample_data(0,1);
					handler->collect_sample_data_signal_at_this_time = 0;
				}
				else if(handler->collect_sample_data_signal_at_this_time_master)
				{
					process_collect_sample_data(1,1);
					handler->LoadMasterWaveForm(handler->collect_sample_data_idx_pole);
					handler->collect_sample_data_signal_at_this_time_master = 0;
				}
				else
				{
					process_collect_sample_data(0,0);
				}
				::Sleep(1);
			}
			handler->UnsetCollectSampleDataSignal();
		}
		handler->SetMachineStatus(_MACH_STATUS_PROJECT_SETTING);
	}
	delete handler->xml_waveform_output;
	return 0;
}

UINT CMediaHandler::ThreadMsgInbound(LPVOID _mothod)
{
	SetThreadAffinityMask( NULL, 1 << 3 );  
	
	// = 0;
	tinyxml2::XMLDocument* doc;
	char* mybuffer = new char[1024*1024];
	while(true)
	{  
		while(!namedPipe->EmptyQueueInbound())
		{
			CString msg = namedPipe->GetQueueInboundDataAndPop();
			//OutputDebugString(msg);
			// 여기서 msg 처리하도록
			msg = msg;
			doc = new tinyxml2::XMLDocument();
			CopyCStringToChar(msg,mybuffer);
			if(doc->Parse(mybuffer)==tinyxml2::XML_SUCCESS)
			{
				tinyxml2::XMLNode* top = doc->FirstChild();
				const char* tt = top->Value(); 
				
				if(strcmp(tt,"ping_from_client")==0) // root element가 work일 경우
				{
					handler->IncreasePingCount();

				}
				else if(strcmp(tt,"work")==0) // root element가 ping일 경우
				{
					tinyxml2::XMLElement* operation		= top->FirstChildElement("operation");
					tinyxml2::XMLElement* option		= top->FirstChildElement("option");
					tinyxml2::XMLElement* message_id	= top->FirstChildElement("message_id");
					if(operation!=NULL)
					{
						CString p = CString(operation->GetText());
						if(p==_T(_STR_MACH_STATUS_BLANK))
						{
							handler->GoToBlankStatus();
							p = p;
						}
						else if(p==_T(_STR_MACH_STATUS_PROJECT_SETTING) && option != NULL)
						{
							tinyxml2::XMLElement* project_name = option->FirstChildElement("project_name");
							tinyxml2::XMLElement* filter_name = option->FirstChildElement("filter_name");
							tinyxml2::XMLElement* sw_name = option->FirstChildElement("sw_name");
							handler->LoadProject(CString(project_name->GetText()));
							if(filter_name!=NULL && sw_name!=NULL)
							{
								handler->LoadProjectAndFilterTuning(
									CString(project_name->GetText()),
									CString(filter_name->GetText()),
									CString(sw_name->GetText()));

							}
							p = p;
						}
					}

					tt = tt;
				}
				else if(strcmp(tt,"input_data_set")==0) // 기타
				{
					// 외부 파형 읽기
					

					handler->LoadWaveFormFromXML( doc, handler->external_waveform);
					handler->external_waveform.data_source = 2;
					handler->external_waveform.time = CTime::GetCurrentTime();

					// handler->last_waveform = handler->external_waveform;
					int i;
					i = 0;
				}
				else if(strcmp(tt,"data_source_change")==0)
				{
					tinyxml2::XMLElement* mode		= top->FirstChildElement("mode");
					if(mode!=NULL)
					{
						CString pp = CString(mode->GetText());
						int nMode = _ttoi(pp);
						handler->SetDataSourceMode(nMode);
					}
				}
				// roi: region of interest 처럼
				// poi: pole of interest  만듬
				else if(strcmp(tt,"poi_source_change")==0)
				{
					tinyxml2::XMLElement* idx_pole		= top->FirstChildElement("idx_pole");
					if(idx_pole!=NULL)
					{
						CString pp = CString(idx_pole->GetText());
						int nMode = _ttoi(pp);
						project->SetRTTargetPole(nMode);
					}
				}
				else if(strcmp(tt,"machine")==0)
				{
					tinyxml2::XMLElement* operation		= top->FirstChildElement("operation");
					if(operation!=NULL)
					{
						CString pp = CString(operation->GetText());
						if(pp==L"START")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n기계 START 명령어 발생\r\n");
							handler->StartPrediction();
						}
						else if(pp==L"STOP")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n기계 STOP 명령어 발생\r\n");
							handler->StopPrediction();
						}
						else if(pp==L"CYCLE")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n 기계 CYCLE 명령어 발생\r\n");
							handler->GoCyclePrediction();
						}
						else if(pp==L"COLLECT_START")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n기계 COLLECT_START 명령어 발생\r\n");
							handler->StartCollecting();
						}
						else if(pp==L"COLLECT_STOP")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n 기계 COLLECT_STOP 명령어 발생\r\n");
							handler->StopCollecting();
						}
						else if(pp==L"LEARNING_START")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n기계 LEARNING_START 명령어 발생\r\n");
							handler->StartLearning();
						}
						else if(pp==L"LEARNING_STOP")
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n 기계 LEARNING_STOP 명령어 발생\r\n");
							handler->StopLearning();
						}
					}
				}
				else if(strcmp(tt,"request_data")==0)
				{
					tinyxml2::XMLElement* req_xml	= top->FirstChildElement("request");
					tinyxml2::XMLElement* msg_xml	= top->FirstChildElement("message_id");
					if(req_xml!=NULL)
					{
						CString request_data  = CString(req_xml->GetText());
						CString request_msgid = L"";
						if(msg_xml!=NULL)
						{
							request_msgid  = CString(msg_xml->GetText());
						}
						if(request_data==L"info_project_property")
						{

							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n요청 info_project_setting 명령어 발생\r\n");
							handler->PushServiceQueue(request_data, request_msgid);
						}
					} 
				}
				else if(strcmp(tt,"set_pole")==0)
				{
					tinyxml2::XMLElement* target_pole_xml	= top->FirstChildElement("target_pole_idx");
					tinyxml2::XMLElement* msg_xml	= top->FirstChildElement("message_id");
					if(target_pole_xml!=NULL)
					{
						int target_pole;
						sscanf(target_pole_xml->GetText(),"%d",&target_pole);
						target_pole = target_pole;
						project->SetRTTargetPole(target_pole);
					}
					tt = tt;
				}
				else if(strcmp(tt,"change_project_data_directory")==0)
				{
					CString dstr;
					tinyxml2::XMLElement* directory_xml	= top->FirstChildElement("directory");
					tinyxml2::XMLElement* msg_xml	= top->FirstChildElement("message_id");
					if(directory_xml!=NULL)
					{
						if(handler->IsRunning())
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
							OutputDebugString(L"작동중일 때는 변경되지 않습니다");
						}
						else
						{
							CString target_directory;
							target_directory = CString(directory_xml->GetText());
							project->SetProjectDataDirectory(target_directory);
							project->UnsetProjectAndFilterInfo();
						}
					}
					tt = tt;
				}
				else if(strcmp(tt,"save_last_waveform_as")==0)
				{
					tinyxml2::XMLElement* file_location_xml	= top->FirstChildElement("file_location");
					tinyxml2::XMLElement* msg_xml	= top->FirstChildElement("message_id");
					CString dstr;
					if(file_location_xml!=NULL)
					{
						if(handler->IsRunning())
						{
							OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
							OutputDebugString(L"작동중일 때는 최종 파형을 저장하지 않습니다.");
						}
						else
						{
							CString file_loc; 
							handler->GenerateInputDataSetXML(file_location_xml->GetText(), NULL,1024);

						}
					}
				}
				else if(strcmp(tt,"load_master")==0)
				{
					tinyxml2::XMLElement* idx_pole_xml		= top->FirstChildElement("idx_pole");
					tinyxml2::XMLElement* degree_xml		= top->FirstChildElement("degree");
					tinyxml2::XMLElement* type_xml			= top->FirstChildElement("type");
					tinyxml2::XMLElement* option_xml		= top->FirstChildElement("option");
					tinyxml2::XMLElement* msg_xml			= top->FirstChildElement("message_id");
					CString dstr;
					if(project->GetProjectName()==L"" ||
						project->GetFilterName()==L"" ||
						project->GetSWName()==L"")
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 명령어가 들어왔지만 프로젝트가 정확히 로드되어 있지 않음");
					} 
					else if(idx_pole_xml==NULL || 
						degree_xml == NULL ||
						type_xml==NULL)
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 명령어 중 최소 요건이 맞지 않음");
					}
					else if(!handler->IsRunning())
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 실행 불가(기계가 동작중이지 않음)");
					}
					else if(handler->GetMachineStatus(dstr)!=MACHINE_STATUS::_MACH_STATUS_COLLECTING_SAMPLE_DATA)
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 수집 상태가 아님");
					}
					else if(CString(type_xml->GetText()).Trim()==
						project->GetSWName())
					{
						int idx_pole, degree, product_type;
						idx_pole_xml->QueryIntText(&idx_pole);
						degree_xml->QueryIntText(&degree);
						
						handler->LoadMasterWaveForm(idx_pole);
					}
					else
					{

						CString dstr;
						dstr.Format(L"현재 중개 프로그램에 등록된 SW이름과 요청이 다릅니다. 등록된 SW 이름: %s",
							project->GetSWName());

						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(dstr);
					}

				}
				else if(strcmp(tt,"do_measurement")==0)
				{
					tinyxml2::XMLElement* idx_pole_xml		= top->FirstChildElement("idx_pole");
					tinyxml2::XMLElement* degree_xml		= top->FirstChildElement("degree");
					tinyxml2::XMLElement* type_xml			= top->FirstChildElement("type");
					tinyxml2::XMLElement* product_type_xml	= top->FirstChildElement("product_type");
					tinyxml2::XMLElement* option_xml		= top->FirstChildElement("option");
					tinyxml2::XMLElement* _is_master		= top->FirstChildElement("_is_master");
					tinyxml2::XMLElement* msg_xml			= top->FirstChildElement("message_id");
					CString dstr;
					if(project->GetProjectName()==L"" ||
						project->GetFilterName()==L"" ||
						project->GetSWName()==L"")
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 명령어가 들어왔지만 프로젝트가 정확히 로드되어 있지 않음");
					} 
					else if(idx_pole_xml==NULL || 
						degree_xml == NULL ||
						type_xml==NULL ||
						product_type_xml==NULL)
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 명령어 중 최소 요건이 맞지 않음");
					}
					else if(!handler->IsRunning())
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 실행 불가(기계가 동작중이지 않음)");
					}
					else if(handler->GetMachineStatus(dstr)!=MACHINE_STATUS::_MACH_STATUS_COLLECTING_SAMPLE_DATA)
					{
						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(L"do_measurement 수집 상태가 아님");
					}
					else if(CString(type_xml->GetText()).Trim()==
						project->GetSWName())
					{
						int idx_pole, degree, product_type;
						idx_pole_xml->QueryIntText(&idx_pole);
						degree_xml->QueryIntText(&degree);
						product_type_xml->QueryIntText(&product_type);

						bool _is_master_value;
						_is_master->QueryBoolText(&_is_master_value);

						if(_is_master_value)
						{
							handler->SetCollectMasterDataSignalAtThisTime(idx_pole);
						}
						else
						{
							handler->SetCollectSampleDataSignalAtThisTime(idx_pole,degree, product_type);
						}

					}
					else
					{

						CString dstr;
						dstr.Format(L"현재 중개 프로그램에 등록된 SW이름과 요청이 다릅니다. 등록된 SW 이름: %s",
							project->GetSWName());

						OutputDebugString(L"CMediaHandler::ThreadMsgInbound\r\n");
						OutputDebugString(dstr);
					}
				}
				else
				{
					tt = tt;
				}
			}


			delete doc;
		}
		::Sleep(1);
	}
	delete mybuffer;

	return 0;
}
UINT CMediaHandler::ThreadOnlyForFetch(LPVOID _mothod)
{
	while(true)
	{
		if(handler->exit_signal) break;
		::Sleep(1);


		if(handler->auto_fetch_signal)
		{
			handler->auto_fetch_signal = 2;
			//
			// 1단계 
			// 계측기에서 데이터 불러오기
			//
			int retv;
			retv = fetch_waveform();
			if(retv == ANDY_NO)
			{
				OutputDebugString(L"CMediaHandler::process_prediction Failed to Fetch Waveform");
				return ANDY_NO;
			}

			handler->auto_fetch_signal = 0;
		}
	}
	return 0;
}
UINT CMediaHandler::ThreadOnlyForCalculation(LPVOID _mothod)
{
	while(true)
	{
		if(handler->exit_signal) break;
		::Sleep(1);
		if(handler->auto_prediction_signal)
		{
			handler->auto_prediction_signal = 2;


			int total_points = handler->nFinalTotalPoints = project->GetTotalPoints();

			int refl_idx = handler->nFinalReflIdx = project->option_data.nReflectionIdx;
			handler->sFianlRefl = project->option_data.sReflection;

			int tran_idx = handler->nFinalTranIdx = project->option_data.nTransmissionIdx;
			handler->sFianlTran = project->option_data.sTransmission;


			//
			// 2단계 
			// 데이터 중 필요한 것만 쓸어오기
			//
			prediction_input_count = total_points;
			int retv = process_waveform_after_fetch(false);

	
			// handler->CopyPresentFileDataTo(total_points,tran_idx,prediction_input_waveform_tran);

			//
			// 3단계 
			// building character waveform, refl, tran 모두 고려해야 함
			//  
			int nCountPointsCharacter = 0;


			double er = handler->GetErrorRate(
				2, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran);


			double* dest_character = handler->BuildCharacterWaveform(
				nCountPointsCharacter,
				project->option_data.nReflectionLearningType, 
				prediction_input_waveform_refl, 
				total_points,

				project->option_data.nTransmissionLearningType,  
				prediction_input_waveform_tran, 
				total_points,
				true,
				er,
				NULL,
				NULL, 0, true); 


			//
			// 4단계 
			// 예측단계
			//
			if(handler->GetPredictionMode()==0) // predict by c/c++ lib
			{
				memcpy(handler->final_waveform_refl, prediction_input_waveform_refl, sizeof(prediction_input_waveform_refl));
				memcpy(handler->final_waveform_tran, prediction_input_waveform_tran, sizeof(prediction_input_waveform_tran));
		
				if(total_points>0)
				{
					/*
					handler->predict(
						total_points,
						prediction_input_waveform_refl, 
						total_points,
						prediction_input_waveform_tran);
					*/
					handler->predict(
						nCountPointsCharacter, 
						dest_character);
					handler->IncreaseFetchCount();
				}
			}
			else if(handler->GetPredictionMode()==1) // predict by r script
			{
				handler->predictByRScript();
			}
			else
			{
				AfxMessageBox(L"이럴수없어");
			}

			//
			// 5단계 
			// 데이터 출력
			//
			handler->GenerateInputDataSetXML(NULL, handler->xml_waveform_output, 1024*1024);
			namedPipe->PushQueueOutbound(CString(handler->xml_waveform_output)); 
			handler->aFinalErrorRate[0] = handler->GetErrorRate(
				0, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);

			handler->aFinalErrorRate[1] = handler->GetErrorRate(
				1, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);

			handler->aFinalErrorRate[2] = handler->GetErrorRate(
				2, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);

			handler->aFinalErrorRate[3] = handler->GetErrorRate(
				3, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);

			handler->aFinalErrorRate[4] = handler->GetErrorRate(
				4, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);

			handler->aFinalErrorRate[5] = handler->GetErrorRate(
				5, refl_idx, tran_idx, total_points,  prediction_input_waveform_refl, total_points, prediction_input_waveform_tran
				);


			// 정리
			delete dest_character;

			handler->auto_prediction_signal = 0;
		}
	}
	return 0;
}
UINT CMediaHandler::ThreadServiceQueue(LPVOID _mothod)
{
	SetThreadAffinityMask( NULL, 1 << 3 );  

	// = 0;
	tinyxml2::XMLDocument* doc;
	char* mybuffer = new char[1024*1024];
	while(true)
	{  
		while(!handler->EmptyServiceQueue())
		{
			CString order, msg_id;
			int retv= handler->PopServiceQueue(order,msg_id);
			if(order==L"info_project_property")
			{
				handler->GenerateInfoProjectPropertyXML(NULL,mybuffer,1024*1024);
				namedPipe->PushQueueOutbound(CString(mybuffer));
				order = order;
			}
		}
		::Sleep(1);
	}
	delete mybuffer;

	return 0;
}




void CMediaHandler::predict( int nTotalPointsCharacter, double* character_array )
{ 
	if(project->GetSWName()==L"RT")
	{
		handler->PredictRT_with_rprop(
			project->GetRTTargetPole(),
			nTotalPointsCharacter,
			character_array);
	}
	else if(project->GetSWName()==L"FT")
	{
		handler->PredictFTwithRProp(
			nTotalPointsCharacter,
			character_array); 
	}
	else
	{
		OutputDebugString(L"RT, FT외에는 정의되지 않음\r\n");
		::Sleep(100);
	}
}

void CMediaHandler::StartLearning()
{
	if(handler->IsRunning())
	{
		OutputDebugString(L"이미 시작 됨");
		return;
	}
	CString sstr,dstr;
	if(handler->GetMachineStatus(dstr) != MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING)
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\n현재 상태가 %s입니다. 반드시 ::PROJECT_SETTING 단계여야 합니다.\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	if(project->GetFilterName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\n필터 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);

		return;
	}
	if(project->GetSWName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\nSW 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);
		return;
	}

	learning_signal = 1;
}

void CMediaHandler::StopCollecting()
{

	if(!handler->IsRunning())
	{
		OutputDebugString(L"이미 시작 됨");
		return;
	}
	CString sstr,dstr;
	if(handler->GetMachineStatus(dstr) != MACHINE_STATUS::_MACH_STATUS_COLLECTING_SAMPLE_DATA)
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\n현재 상태가 %s입니다. 반드시 ::COLLECTING_SAMPLE_DATA 단계여야 합니다.\r\n",sstr);
		OutputDebugString(dstr);
		return;
	} 

	collect_sample_data_signal = 0;
}

void CMediaHandler::StartPrediction()
{
	if(handler->IsRunning())
	{
		OutputDebugString(L"이미 시작 됨");
		return;
	}
	CString sstr,dstr;
	if(handler->GetMachineStatus(dstr) != MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING)
	{
		dstr.Format(L"CMediaHandler::StartPrediction\r\n현재 상태가 %s입니다. 반드시 ::PROJECT_SETTING 단계여야 합니다.\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	if(project->GetFilterName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartPrediction\r\n필터 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);

		return;
	}
	if(project->GetSWName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartPrediction\r\nSW 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);
		return;
	}

	prediction_signal = 1;
}
void CMediaHandler::GoCyclePrediction()
{

	if(handler->IsRunning())
	{
		OutputDebugString(L"이미 시작 됨");
		return;
	}

	CString sstr,dstr;
	if(handler->GetMachineStatus(dstr) != MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING)
	{
		dstr.Format(L"CMediaHandler::GoCyclePrediction\r\n현재 상태가 %s입니다. 반드시 ::PROJECT_SETTING 단계여야 합니다.\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	if(project->GetFilterName()==L"")
	{
		dstr.Format(L"CMediaHandler::GoCyclePrediction\r\n 필터 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);
		return;
	}
	if(project->GetSWName()==L"")
	{
		dstr.Format(L"CMediaHandler::GoCyclePrediction\r\n SW 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);
		return;
	}

	prediction_once_signal = 1;
}
void CMediaHandler::StopPrediction()
{
	prediction_signal = prediction_once_signal = 0;
}

void CMediaHandler::StopLearning()
{
	CString dstr;
	if(GetMachineStatus(dstr)==MACHINE_STATUS::_MACH_STATUS_BUILD_FILTER_MODE)
	{
		learning_stop_signal = 1;
	}
	else
	{
		OutputDebugString(L"CMediaHandler::StopLearning 기계가 학습중이지 않습니다.");
	}
}

void CMediaHandler::SetIsRunning(int signal)
{
	_is_running = signal;
}

void CMediaHandler::SetCollectSampleDataSignalAtThisTime(int idx_pole, int degree, int product_type)
{
	collect_sample_data_signal_at_this_time = 1;
	collect_sample_data_idx_pole = idx_pole;
	collect_sample_data_degree = degree;
	collect_sample_data_product_type = product_type;
}
void CMediaHandler::UnsetCollectSampleDataSignal()
{
	collect_sample_data_signal = 0;
	collect_sample_data_idx_pole = -1;	
	collect_sample_data_product_type = 1;
	collect_sample_data_product_type = 1;
}
int CMediaHandler::IsRunning()
{
	return _is_running;
}

void CMediaHandler::StartCollecting()
{
	if(handler->IsRunning())
	{
		OutputDebugString(L"이미 시작 됨");
		return;
	}
	CString sstr,dstr;
	if(handler->GetMachineStatus(dstr) != MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING)
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\n현재 상태가 %s입니다. 반드시 ::PROJECT_SETTING 단계여야 합니다.\r\n",sstr);
		OutputDebugString(dstr);
		return;
	}
	if(project->GetFilterName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\n필터 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);

		return;
	}
	if(project->GetSWName()==L"")
	{
		dstr.Format(L"CMediaHandler::StartLearning\r\nSW 이름이 로딩되지 않았습니다.\r\n");
		OutputDebugString(dstr);
		return;
	}

	collect_sample_data_signal = 1;
}

void CMediaHandler::IncreaseFetchCount()
{
	nFetchCount++;
}

void CMediaHandler::IncreasePingCount()
{
	nPingCount++;
}
int CMediaHandler::GetPingCount()
{
	return nPingCount;
}

int CMediaHandler::GetDuration()
{
	return time(0) - start_time;
}

int CMediaHandler::GetMsgCountFromServer()
{
	return general_msg_id_increment;
}
int CMediaHandler::GetFetchCount()
{
	return nFetchCount;
}

void CMediaHandler::SetDataSourceMode(int mode)
{
	project->SetDataSourceMode(mode);
	// this->mode_data_source = mode;
}
void CMediaHandler::SetPredictionMode(int mode)
{
	this->mode_prediction_mode = mode;
}

MACHINE_STATUS CMediaHandler::GetMachineStatus( CString& str )
{
	switch(machine_status)
	{
	case MACHINE_STATUS::_MACH_STATUS_BLANK: str = L"BLANK"; break;
	case MACHINE_STATUS::_MACH_STATUS_BUILD_FILTER_MODE: str = L"BUILD_FILTER_MODE"; break;
	case MACHINE_STATUS::_MACH_STATUS_COLLECTING_SAMPLE_DATA: str = L"COLLECTING_SAMPLE_DATA"; break;
	case MACHINE_STATUS::_MACH_STATUS_MASTER_DATA: str = L"MASTER_DATA"; break;
	case MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING: str = L"PROJECT_SETTING"; break;
	case MACHINE_STATUS::_MACH_STATUS_TUNING_PROCESS: str = L"TUNING_PROCESS"; break;
	}
	return machine_status;
}

void CMediaHandler::GoToBlankStatus()
{
	if(IsRunning())
	{
		OutputDebugString(L"CMediaHandler::GoToBlankStatus 기계 작동시 BLANK 상태로 갈 수 없습니다.");
		return;
	}
	project->UnsetProjectAndFilterInfo();
	SetMachineStatus(MACHINE_STATUS::_MACH_STATUS_BLANK);
}

void CMediaHandler::SetMachineStatus( MACHINE_STATUS next_status )
{
	if(this->machine_status == next_status) 
	{
		return;
	}
	this->machine_status = next_status;
}

int CMediaHandler::GetDataSourceMode()
{
	return project->GetDataSourceMode();
	// return mode_data_source;
}
int CMediaHandler::GetPredictionMode()
{
	return mode_prediction_mode;
}

void CMediaHandler::predictByRScript()
{
	double output_result[1000];
	int output_flag[1000];
	int output_result_flag;


	CString prediction_path;
	CString baegopa_file;
	baegopa_file.Format(L"C:\\filter_tuning_program\legacy\\input_touched.txt");

	// input_touched.txt가 사라질 때 까지 기다림
	// 파일이 사라졌다는 의미는 output_pole이 업데이트 되었다는 뜻 임
	while(true)
	{
		Sleep(10);
		if(PathFileExists(baegopa_file)) continue;

		break;

	}
	CString output_path;
	output_path.Format(L"C:\\filter_tuning_program\\legacy\\output_pole.csv");


	ifstream infile(output_path);
	std::string line;
	int cnt = 0;
	int cnt2 = 0;
	while (std::getline(infile, line))
	{
		line = line;
		cnt++;
		if(cnt==2)
		{ 
			istringstream is(line);
			string part;
			while (getline(is, part, ','))
			{	
				cout << part << endl;
				if(cnt2>=1)
				{
					output_result[cnt2-1] = ::atof(part.c_str());
					output_result[cnt2-1] *= 10.0;
				}
				cnt2++;
			}
			break;
		}
	}	
	int i;
	double maxv[2] = {0.0, };
	int maxi[2] = {0, };

	for(i=0;i<19;i++)
	{
		if(i%2==0 && i!=18 && i!= 8)
		{
			if(maxv[0]<=abs(output_result[i]))
			{
				maxv[0] = abs(output_result[i]);
				maxi[0] = i;
			}
		}


		if(maxv[1]<=abs(output_result[i]))
		{
			maxv[1] = abs(output_result[i]);
			maxi[1] = i;
		}
	}
	memset(output_flag,0,sizeof(output_flag));
	output_flag[maxi[0]] = 1;
	output_flag[maxi[1]] = 1;

	output_result_flag = 1;
	infile.close(); 



	baegopa_file.Format(L"C:\\filter_tuning_program\\legacy\\input_touched.txt");

	// input_touched.txt가 사라질 때 까지 기다림
	// 파일이 사라졌다는 의미는 output_pole이 업데이트 되었다는 뜻 임
	while(true)
	{
		Sleep(10);
		if(PathFileExists(baegopa_file)) continue;

		break;
	}



	output_path.Format(L"C:\\filter_tuning_program\\legacy\\output_pole.csv");


	ifstream infile22(output_path);
	line;
	cnt = 0;
	cnt2 = 0;
	while (std::getline(infile22, line))
	{
		line = line;
		cnt++;
		if(cnt==2)
		{ 
			istringstream is(line);
			string part;
			while (getline(is, part, ','))
			{	
				cout << part << endl;
				if(cnt2>=1)
				{
					output_result[cnt2-1] = ::atof(part.c_str());
					output_result[cnt2-1] *= 10.0;
				}
				cnt2++;
			}
			break;
		}
	}	
	double maxv22[2] = {0.0, };
	int maxi22[2] = {0, };

	for(i=0;i<19;i++)
	{
		if(i%2==0 && i!=18 && i!= 8)
		{
			if(maxv22[0]<=abs(output_result[i]))
			{
				maxv22[0] = abs(output_result[i]);
				maxi22[0] = i;
			}
		}
		prediction_output_result[i] = output_result[i];


		if(maxv[1]<=abs(output_result[i]))
		{
			maxv[1] = abs(output_result[i]);
			maxi[1] = i;
		}
	}
	memset(output_flag,0,sizeof(output_flag));
	output_flag[maxi[0]] = 1;
	output_flag[maxi[1]] = 1;

	output_result_flag = 1;
	infile22.close(); 


}

void CMediaHandler::PredictFTwithRProp( int ccount, double* character_array )
{

	//const double R_SCALE_MAX = 250;
	//const double C_SCALE_MAX = 125;

	const double R_SCALE_MAX = 10;
	const double C_SCALE_MAX = 10;

	if(project->GetProjectName()==L"") return;
	if(project->GetFilterName()==L"") return;
	if(project->GetSWName()!=L"FT") return;

	int num_input	= ccount-1; 
	int num_hidden	= 50;
	int num_output	= project->GetTotalPoles();
	int num_poles	= project->GetTotalPoles();
	int num_train	= 809;
	double learning_rate = 0.2;

	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::PredictFTwithRProp\r\nR과 C외에는 준비되지 않았습니다"); }
	} 


	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );
	

	if(strcmp(loaded_fileloc,fileloc)==0)
	{
		// nothing to do
	}
	else
	{
		strcpy(loaded_fileloc,fileloc);
	//	if(rProp!=NULL) delete rProp;
		rProp = new Rprop(num_input, num_hidden, num_output, num_train);
		rProp->loadWeights(fileloc);


	}


	
	//Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);

	// double* charater;
	// charater = new double[num_input];
	int i;

	// 161019
	// rProp->loadWeights(fileloc);
	
	double *output = new double[num_output];
	rProp->ComputeOutputs(character_array, output);
	//FNN->Test(character_array, output);

	for(i=0;i<project->GetTotalPoles();i++)
	{
		prediction_output_result[i] = (output[i] *(maxs[i]-mins[i]) +mins[i])*100;	
	}
		

	double maxv[2] = {0.0, };
	int maxi[2] = {0, };

	for(i=0;i<project->GetTotalPoles();i++)
	{
		if((project->GetPolePropertyAt(i+1)==L"R" ||
			project->GetPolePropertyAt(i+1)==L"C") &&
			maxv[0]<=abs(prediction_output_result[i])
			)
		{
			maxv[0] = abs(prediction_output_result[i]);
			maxi[0] = i;
		}



		if(project->GetPolePropertyAt(i+1)==L"R" &&
			maxv[1]<=abs(prediction_output_result[i]))
		{

			if(maxv[1]<=abs(prediction_output_result[i]))
			{
				maxv[1] = abs(prediction_output_result[i]);
				maxi[1] = i;
			}
		}
		prediction_output_flag[i] = false;
	}
	prediction_output_flag[maxi[0]] = 1;
	prediction_output_flag[maxi[1]] = 1;

	//완전 종료시 날려야함 2016-10-19 
	//delete rProp;
	//delete FNN;
	delete[] output;
}

double* CMediaHandler::BuildCharacterWaveform(
	int& character_count, 
	const int refl_type, const double (*waveform_refl)[2048], const int waveform_refl_count,
	const int tran_type, const double (*waveform_tran)[2048], const int waveform_tran_count,
	bool use_error_rate, double fErrorRate,
	double* master_array,
	double* augmented_array, const int augmented_array_count, bool use_scaling)
{
	double* dest_character;
	int i,j;
	character_count = 0;
	int ccount_refl = 0;
	int ccount_tran = 0;
	if(refl_type>=0)
	{
		for(i=0;i<waveform_refl_count;i++)
		{
			bool in_range_flag = false;
			if(!project->option_data.nUseRangePred) in_range_flag = true;
			else
			{
				for(j=0;j<TOTAL_PRED_RANGE;j++)
				{
					if(!project->pred_data[j].nExist) break; 
					if(project->pred_data[j].nStartFreq <= waveform_refl[IDX2_FREQ][i] &&
						waveform_refl[IDX2_FREQ][i] <= project->pred_data[j].nStopFreq)
					{
						in_range_flag = true;
						break;
					}
				}
			}
			if(in_range_flag) ccount_refl ++;
		}
	}

		
	if(tran_type>=0)
	{
		for(i=0;i<waveform_tran_count;i++)
		{
			bool in_range_flag = false;
			if(!project->option_data.nUseRangePred) in_range_flag = true;
			else
			{
				for(j=0;j<TOTAL_PRED_RANGE;j++)
				{
					if(!project->pred_data[j].nExist) break; 
					if(project->pred_data[j].nStartFreq <= waveform_tran[IDX2_FREQ][i] &&
						waveform_tran[IDX2_FREQ][i] <= project->pred_data[j].nStopFreq)
					{
						in_range_flag = true;
						break;
					}
				}
			}
			if(in_range_flag) ccount_tran ++;
		}
	}


	int increment = 0;
	character_count = 0;

	if(refl_type==0) character_count += ccount_refl * 2;
	else if(refl_type==1) character_count += ccount_refl;
	else if(refl_type==2) character_count += ccount_refl * 2;
	else if(refl_type==3) character_count += ccount_refl;

	if(tran_type==0) character_count += ccount_tran * 2;
	else if(tran_type==1) character_count += ccount_tran;
	else if(tran_type==2) character_count += ccount_tran * 2;
	else if(tran_type==3) character_count += ccount_tran;

	character_count += augmented_array_count;

	if(use_error_rate) character_count ++;

	dest_character = new double[character_count+1];



	if(refl_type>=0)
	{
		for(i=0;i<waveform_refl_count;i++)
		{
			bool in_range_flag = false;
			if(!project->option_data.nUseRangePred) in_range_flag = true;
			else
			{
				for(j=0;j<TOTAL_PRED_RANGE;j++)
				{
					if(!project->pred_data[j].nExist) break; 
					if(project->pred_data[j].nStartFreq <= waveform_refl[IDX2_FREQ][i] &&
						waveform_refl[IDX2_FREQ][i] <= project->pred_data[j].nStopFreq)
					{
						in_range_flag = true;
						break;
					}
				}
			}

			if(!in_range_flag) continue;

			if(refl_type==0)
			{
				dest_character[increment] = waveform_refl[IDX2_REAL][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;

				dest_character[increment] = waveform_refl[IDX2_IMAGE][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;
			}
			else if(refl_type==1)
			{
				dest_character[increment] = waveform_refl[IDX2_MAG][i];
				//if(use_scaling) dest_character[increment] = (dest_character[increment]+40) /80;
				increment++;
			}
			else if(refl_type==2)
			{ 
								
					dest_character[increment] = waveform_refl[IDX2_PHASE][i];
					if(use_scaling)
					{
						dest_character[increment] = (dest_character[increment]) / 360.0;
						dest_character[increment] += 0.5;
					}
					
					//increment++;
				
				
					dest_character[increment+waveform_refl_count] = waveform_refl[IDX2_MAG][i];
					if(use_scaling) dest_character[increment+waveform_refl_count] = (dest_character[increment+waveform_refl_count] + 40.0) / 80.0;					
					increment++;

			}
			else if(refl_type==3)
			{
				dest_character[increment] = waveform_refl[IDX2_PHASE][i];
				//if(use_scaling) dest_character[increment] = (dest_character[increment]+180) / 360;
				if(use_scaling){
					dest_character[increment] = (dest_character[increment]) / 360.0;
					dest_character[increment] += 0.5;
				}
				increment++;
			}
		}
	}

	if(tran_type>=0)
	{
		AfxMessageBox(L"Transmission ");
		for(i=0;i<waveform_tran_count;i++)
		{
			bool in_range_flag = false;
			if(!project->option_data.nUseRangePred) in_range_flag = true;
			else
			{
				for(j=0;j<TOTAL_PRED_RANGE;j++)
				{
					if(!project->pred_data[j].nExist) break; 
					if(project->pred_data[j].nStartFreq <= waveform_tran[IDX2_FREQ][i] &&
						waveform_tran[IDX2_FREQ][i] <= project->pred_data[j].nStopFreq)
					{
						in_range_flag = true;
						break;
					}
				}
			}
			if(in_range_flag) ccount_tran ++;

			if(tran_type==0)
			{
				dest_character[increment] = waveform_tran[IDX2_REAL][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;

				dest_character[increment] = waveform_tran[IDX2_IMAGE][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;
			}
			else if(tran_type==1)
			{
				dest_character[increment] = waveform_tran[IDX2_MAG][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;
			}
			else if(tran_type==2)
			{
				dest_character[increment] = waveform_tran[IDX2_MAG][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;

				dest_character[increment] = waveform_tran[IDX2_PHASE][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;
			}
			else if(tran_type==3)
			{
				dest_character[increment] = waveform_tran[IDX2_PHASE][i];
				if(use_scaling) dest_character[increment] = (dest_character[increment]+2.5) / 5;
				increment++;
			}
		}
	}

	if(use_error_rate)
	{
		dest_character[increment++] = fErrorRate/increment; ///increment;		
	}
	else{

		if(master_array != NULL)
		{
			double error_rate=0;
			for(i=0;i<increment;i++)
			{
				error_rate += (dest_character[i] - master_array[i]) * (dest_character[i] - master_array[i]);
			}	
			dest_character[increment++] = error_rate/increment; ///increment;		
		}
	}

	if(augmented_array_count>0 && augmented_array != NULL)
	{
		for(i=0;i<augmented_array_count;i++)
		{
			dest_character[increment++] = augmented_array[i];
		}		
	}
	if(increment==530)
	{
		double mytemparray[530];
		memcpy(mytemparray,dest_character,sizeof(mytemparray));
		mytemparray[0] = mytemparray[0];
	}
	return dest_character;
}

int CMediaHandler::GenerateInfoProjectPropertyXML( char* xml_location, char* dest, int max_len, const char* feedback_msg )
{
	int i,j,k;
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* info = doc->InsertEndChild( doc->NewElement( "info_project_property" ) );
	tinyxml2::XMLElement* sub[3] = { 
		doc->NewElement( "projects" ),			// 0
		doc->NewElement( "message_id" ),		// 1
		doc->NewElement( "origin_msg")
	};
	char buf_output[128];
	CString sstr;


	std::list<CString> mylist = project->GetProjectNameList();
	for(std::list<CString>::iterator itor = mylist.begin();
		itor != mylist.end();
		itor++)
	{
		tinyxml2::XMLElement* sub_project = doc->NewElement("project");
		char bufdata[128];
		std::list<CString> mylist_filter = project->GetFilterListByProject(*itor);
		CString sProjName = *itor;

		CopyCStringToChar(sProjName,bufdata);
		sub_project->SetAttribute("name", bufdata);

		for(std::list<CString>::iterator itor2 = mylist_filter.begin();
			itor2 != mylist_filter.end();
			itor2++)
		{ 
			tinyxml2::XMLElement* sub_filter = doc->NewElement("filter");
			char bufdata2[128];
			CString sFilterName = *itor2;

			CopyCStringToChar(sFilterName,bufdata2);
			sub_filter->SetAttribute("name", bufdata2);

			std::list<CString> mylist_swname = project->GetSWNameListByProjectAndFilter(*itor, *itor2);

			for(std::list<CString>::iterator itor3 = mylist_swname.begin();
				itor3 != mylist_swname.end();
				itor3++)
			{
				tinyxml2::XMLElement* sub_filter_sw;
				CString sSWName = *itor3;
				char bufdata3[128];
				sub_filter_sw = doc->NewElement("sw");

				 
				CopyCStringToChar(sSWName,bufdata3);
				sub_filter_sw->SetAttribute("name", bufdata3);


				tinyxml2::XMLElement* sub_filter_sw_data[6] = { 
					doc->NewElement( "master_data" ), 
					doc->NewElement( "sample_data" ), 
					doc->NewElement( "weight_data" ), 
					doc->NewElement( "reflection" ), 
					doc->NewElement( "transmission" ), 
					doc->NewElement( "poles" )
				};
				int nReqData, nExistData;
				sub_filter_sw_data[0]->SetText(
					project->HasMasterDataFromFile(sProjName, sFilterName, sSWName)
					);

				sub_filter_sw_data[1]->SetText(
					project->HasSampleDataFromFile(sProjName,sFilterName,sSWName, &nReqData, &nExistData)
					);
				sub_filter_sw_data[1]->SetAttribute("requirement",nReqData);
				sub_filter_sw_data[1]->SetAttribute("exist",nExistData);
				

				sub_filter_sw_data[2]->SetText(
					project->HasWeightDataFromFile(sProjName,sFilterName,sSWName, &nReqData, &nExistData)
					);
				sub_filter_sw_data[2]->SetAttribute("requirement",nReqData);
				sub_filter_sw_data[2]->SetAttribute("exist",nExistData);

				CString sRef = project->GetReflectionFromFile(sProjName,sFilterName,sSWName);
				CString sTrans = project->GetTransmissionFromFile(sProjName,sFilterName,sSWName);
				CopyCStringToChar(sRef,bufdata3);
				sub_filter_sw_data[3]->SetText(bufdata3);
				CopyCStringToChar(sTrans,bufdata3);
				sub_filter_sw_data[4]->SetText(bufdata3);

				sub_filter_sw_data[5]->SetText(
					project->GetTotalPolesFromFile(sProjName,sFilterName,sSWName)
					);
				for(i=0;i<6;i++)
				{
					sub_filter_sw->InsertEndChild(sub_filter_sw_data[i]);
				}
				sub_filter->InsertEndChild(sub_filter_sw);
			} 

			sub_project->InsertEndChild(sub_filter);
		}
		sub[0]->InsertEndChild(sub_project);
	}

	char msgid[128];
	sprintf(msgid,"%d_%d",
		++general_msg_id_increment, 
		time(NULL));
	sub[1]->SetText(msgid);

	for(i=0;i<3;i++)
	{
		info->InsertEndChild( sub[i] );
	}
	std::stringstream ss;
	tinyxml2::XMLPrinter printer;
	doc->Print( &printer );
	ss << printer.CStr();


	if(xml_location!=NULL)
	{
		doc->SaveFile(xml_location,true);
	}


	int len = strlen(printer.CStr());
	if(len > max_len) len = max_len;
	memset(dest,0,sizeof(char)*max_len);
	memcpy(dest, printer.CStr(),len);
	delete doc;
	return len;
}

void CMediaHandler::PushServiceQueue( CString order, CString feedback_msg_id )
{
	EnterCriticalSection(&cs_service_queue);
	struct SServiceQueueData mydata;
	mydata.order = order;
	mydata.feedback_msg_id = feedback_msg_id;
	qServiceQueue.push(mydata);
	LeaveCriticalSection(&cs_service_queue);

}

int CMediaHandler::PopServiceQueue( CString& order, CString& feedback_msg_id )
{
	int retv;
	EnterCriticalSection(&cs_service_queue);
	if(EmptyServiceQueue()) retv = 1;
	else 
	{
		struct SServiceQueueData mydata;
		mydata = qServiceQueue.back();
		order = mydata.order;
		feedback_msg_id = mydata.feedback_msg_id;
		retv = 0;
		qServiceQueue.pop();
	}
	LeaveCriticalSection(&cs_service_queue);

	return retv;
}

bool CMediaHandler::EmptyServiceQueue()
{
	bool retv;
	EnterCriticalSection(&cs_service_queue);
	retv = qServiceQueue.empty();
	LeaveCriticalSection(&cs_service_queue);

	return retv;
}

void CMediaHandler::SetCollectMasterDataSignalAtThisTime(int idx_pole)
{
	collect_sample_data_idx_pole = idx_pole;
	collect_sample_data_signal_at_this_time_master = 1;
}

int CMediaHandler::GetLearningStopSignal()
{
	return learning_stop_signal;
}

int CMediaHandler::DoMeasurementMasterFT( int idx_pole, int product_type )
{
	for(int i=0;i<2;i++)
	{
		char xml_directory[1024]; 
		CString fileloc = project->GetFTMasterFileLoc(idx_pole, i);
		CFileFind finder;
		BOOL bWorking = finder.FindFile(fileloc);
		if(bWorking)
		{
			// 기존 파일이 있으면.old에 백업
			MoveFile(fileloc, fileloc + L".old");
		}

		// Master 파형 저장
		CopyCStringToChar(fileloc, xml_directory);
		GenerateInputDataSetXML(xml_directory,NULL,0); 
	}
	return 0;
}

int CMediaHandler::DoMeasurementMasterRT( int idx_pole, int product_type/*=1*/ )
{
	int i;
	for(i=0;i<2;i++)
	{
		char xml_directory[1024]; 
		CString fileloc = project->GetRTMasterFileLoc(idx_pole,i);
		CFileFind finder;
		BOOL bWorking = finder.FindFile(fileloc);
		if(bWorking)
		{
			// 기존 파일이 있으면.old에 백업
			MoveFile(fileloc, fileloc + L".old");
		}

		// Master 파형 저장
		CopyCStringToChar(fileloc, xml_directory);
		GenerateInputDataSetXML(xml_directory,NULL,0);
	}
	return 0;
}

int CMediaHandler::GetLearningTime()
{
	if(handler->learning_signal==0) 
	{
		return 0;
	}
	return time(0) - learning_start_time;

}

bool CMediaHandler::RankComp( const CString& const_lhs, const CString& const_rhs )
{
	CString lhs = const_lhs;
	CString rhs = const_rhs;
	CString lhs_1,lhs_2;
	CString rhs_1,rhs_2;
	AfxExtractSubString( lhs_1, lhs, 0, '_');
	AfxExtractSubString( lhs_2, lhs, 1, '_');
	AfxExtractSubString( lhs_2, lhs_2, 0, '.');


	AfxExtractSubString( rhs_1, rhs, 0, '_');
	AfxExtractSubString( rhs_2, rhs, 1, '_');
	AfxExtractSubString( rhs_2, rhs_2, 0, '.');
	if(lhs_1 == L"wf" && rhs_1 == L"wf")
	{
		int lhs_2_int;
		int rhs_2_int;
		lhs_2_int = _ttoi(lhs_2);
		rhs_2_int = _ttoi(rhs_2);
		if(lhs_2_int == rhs_2_int)
		{
			return const_lhs.CompareNoCase(const_rhs)<0;
		}
		else
		{
			return lhs_2_int < rhs_2_int;
		}
	}
	else
	{
		return const_lhs.CompareNoCase(const_rhs)<0;
	}

	return 0;
}
