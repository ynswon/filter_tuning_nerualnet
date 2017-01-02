#include "StdAfx.h"
#include "NetworkAnalyzerCtr.h"
 
CNetworkAnalyzerCtr::CNetworkAnalyzerCtr(void)
{
	// 한번 부를 때 마다 하나씩 증가
	data_fetch_sqc = 0;

	_is_opened = 0;
	count_data = 310;
	OPERATION_MODE = 3;

	// project name은 operation mode가 2일 때만 유효

	project_name = "pole19";
	filter_name = "LOW";
	tuning_phase_name = "FT";
	
	if(project_name=="pole19") total_poles = 19;
	else total_poles = 5;

	
	// project->LoadProjectFromINI	(project_name, filter_name, tuning_phase_name);



	// 상주 프로세스 predict_100.bat
	/*
	CString prediction_path;
	prediction_path.Format("C:\\mywork\\script_ver2\\%s\\predict_100.bat",project_name);
	STARTUPINFO si = {0,};    //구조체 선언, 초기화
	PROCESS_INFORMATION pi = {0,};
	BOOL state;
	*/

	InitializeCriticalSection(&cs);
//	Typenetworkanal = AgilentENA;
	Typenetworkanal = Rohde_AND_Schwarz;
}


CNetworkAnalyzerCtr::~CNetworkAnalyzerCtr(void)
{
	DeleteCriticalSection(&cs);
	close();
}
void CNetworkAnalyzerCtr::read_master()
{

}
int CNetworkAnalyzerCtr::GetTotalPoints()
{
	return project->GetTotalPoints();
}
void CNetworkAnalyzerCtr::meas(int mode, int& count, double (*waveform)[5][2048],int tracecount )
{
#ifdef USE_VISA
	if(!_is_opened)
	{
		open();
	}

	ViPUInt32 pp;
	char buf [1024] = {0};



	int my_total_point;
	my_total_point = project->GetTotalPoints();

	int argument[2];
	argument[0] = mode/16;
	argument[1] = mode%16;
	pp = (ViPUInt32)malloc(sizeof(ViPUInt32));
	int count_data = 0;
	int order = project->option_data.aTraceOrderMapping[argument[0]][argument[1]];
	// CString sScalingFunction = project->option_data.sScalingFunction[argument[0]][argument[1]];
	if(order<=0)
	{
		AfxMessageBox(L"맵핑이 되어 있지 않습니다");
	}


	if(Typenetworkanal== Rohde_AND_Schwarz )
	{
		sprintf(buf, "CALC1:PAR:SEL \"MyMeas_1_%d_S%d%d\"\%c",order,argument[0]+1,argument[1]+1,0x0A);
		mywrite(buf,pp);
		sprintf(buf, "CALC1:DATA? SDAT%c",0x0A);
		mywrite(buf,pp);

	}else if (Typenetworkanal ==AgilentENA)
	{
		sprintf(buf, "CALC1:PAR%d:SEL\n",tracecount+1);
		mywrite(buf,pp);
		sprintf(buf, "CALC1:DATA:SDAT?\n");
		mywrite(buf,pp);
		
	}
	/*sprintf(buf, "CALC1:PAR:SEL \"MyMeas_1_%d_S%d%d\"\%c",order,argument[0]+1,argument[1]+1,0x0A);
	mywrite(buf,pp);
	sprintf(buf, "CALC1:DATA? SDAT%c",0x0A);
	mywrite(buf,pp);*/

	viRead (vi, (ViBuf)general_buffer, 1024*1024, pp);

	int n = *pp;
	int i; 

	char *p_token = NULL;
	p_token = strtok(general_buffer,",");

	while(p_token != NULL)
	{
		// real
		double tt;
		sscanf(p_token,"%lf",&tt);
		waveform[mode][IDX2_REAL][count_data] = (double)tt;

		p_token = strtok(NULL, ",");
		if(!p_token) break;
	
		// image
		sscanf(p_token,"%lf",&tt);
		waveform[mode][IDX2_IMAGE][count_data] = (double)tt;

		p_token = strtok(NULL, ",");
		count_data++;
		if(!p_token) break;
	}

	sprintf(buf, "*OPC?%c",0x0A);
	mywrite(buf,pp);
	viRead (vi, (ViBuf)general_buffer, 1024*1024, pp);


	for(i=0;i<my_total_point;i++)
	{
		waveform[mode][IDX2_MAG][i] = - 20.0 * log(sqrt(pow(waveform[mode][IDX2_REAL][i],2)+pow(waveform[mode][IDX2_IMAGE][i],2))) / log(10.0);
		waveform[mode][IDX2_PHASE][i] = 180/M_PI * atan2(waveform[mode][IDX2_IMAGE][i], waveform[mode][IDX2_REAL][i]);
		if(waveform[mode][IDX2_PHASE][i]>180) waveform[mode][IDX2_PHASE][i] -= 360;		
		
		/*if(waveform[mode][IDX2_REAL][i]>0) 
			waveform[mode][IDX2_PHASE][i] = 180/M_PI*atan2(waveform[mode][IDX2_IMAGE][i], waveform[mode][IDX2_REAL][i]);
		else if(waveform[mode][IDX2_IMAGE][i]>0)  
			waveform[mode][IDX2_PHASE][i] = 180.0+180.0/M_PI*atan2(waveform[mode][IDX2_IMAGE][i], waveform[mode][IDX2_REAL][i]);
		else waveform[mode][IDX2_PHASE][i] = 180.0/M_PI*atan2(waveform[mode][IDX2_IMAGE][i], waveform[mode][IDX2_REAL][i])-180.0;*/
		
	}
	present_data_count = count_data;
#else
	present_data_count = 0; 
#endif
}	
int CNetworkAnalyzerCtr::open()
{
#ifdef USE_VISA
	if(!_is_opened)
	{
		CString address = project->GetDataSourceInteralAddress();
		char address_char[128];

		if(address==L"")
		{
			address = L"TCPIP::192.168.0.1::INSTR";
		}
		CopyCStringToChar(address, address_char);

		viOpenDefaultRM(&defaultRM);
		try
		{
			int pp = viOpen(defaultRM, address_char,VI_NULL,VI_NULL,&vi);
			//if(pp==)
			if(pp & _VI_ERROR)
			{
				_is_opened = 0;
			}
			else
			{
				initialize_model_builder(); 
				_is_opened = 1;
			}
		}
		catch (CMemoryException* e)
		{
			e = e;
			return ANDY_NO;
		}
		catch (CFileException* e)
		{
			e = e;
			return ANDY_NO;
		}
		catch (CException* e)
		{
			e = e;
			return ANDY_NO;
		}

		if(_is_opened) return ANDY_YES;
		return ANDY_NO;
	}
	else
	{
		return ANDY_YES;
	}
#else
	return 0;
#endif
}

void CNetworkAnalyzerCtr::get_final_waveform(
	int wafeform_type, 
	int max_count, 
	double* data_freq,
	double* data_real,
	double* data_image,
	double* data_mag,
	double* data_phase)
{
	int i;
	int n;
	n = project->GetTotalPoints();
	if(n < max_count)
	{
		n = max_count;
	}
	for(i=0;i<max_count;i++)
	{ 
		if(data_freq != NULL)	data_freq[i] = present_data[wafeform_type][IDX2_FREQ][i];
		if(data_real != NULL)	data_real[i] = present_data[wafeform_type][IDX2_REAL][i];
		if(data_image != NULL)	data_image[i] = present_data[wafeform_type][IDX2_IMAGE][i];
		if(data_mag != NULL)	data_mag[i] = present_data[wafeform_type][IDX2_MAG][i];
		if(data_phase != NULL)	data_phase[i] = present_data[wafeform_type][IDX2_PHASE][i];
	}
}
int CNetworkAnalyzerCtr::fetch_ver2(
	int operation_mode, 
	int pole, 
	int degree,
	bool use_legacy_compatibility) // pole: 1~15, degree -10~10
{
#ifdef USE_VISA
	data_fetch_sqc++;
	
	if(!_is_opened)
	{
		int retv = open(); 
		if(retv == ANDY_NO) return ANDY_NO;
	}

	if(!_is_opened)
	{
		return ANDY_NO;
	}
	if(degree<0) degree *= -1;

	char buf [1024] = {0};

	int i;
	int mode;
	int my_total_point = project->GetTotalPoints(); 
	std::list<int>::iterator itor;	
	i =0;
	for(itor=project->option_data.aTraceOrderInt.begin();
		itor!=project->option_data.aTraceOrderInt.end();
		itor++)
	{
		int mode, count;
		mode = (*itor);
		
		meas(mode, count, present_data,i);
		i++;//순서대로 보냅시다...
	}

	if(!use_legacy_compatibility)
	{
		for(itor=project->option_data.aTraceOrderInt.begin();
			itor!=project->option_data.aTraceOrderInt.end();
			itor++)
		{
			mode = (*itor);
			data_sqc[mode] = data_fetch_sqc;
			data_cnt[mode] = my_total_point;
		}
	}
	else
	{
		char* var_str ="VAR";

		CString filename_prefix;
		ofstream outFile;
		 

		for(itor=project->option_data.aTraceOrderInt.begin();
			itor!=project->option_data.aTraceOrderInt.end();
			itor++)
		{
			CString directory_pos;
			int argument[2];
			mode = (*itor);
			argument[0] = mode/16;
			argument[1] = mode%16;

			if(operation_mode==0)
			{
				filename_prefix.Format(
					L"C:\\filter_tuning_program\\legacy\\input_s%d%d",
					argument[0]+1,
					argument[1]+1);
			}

			outFile = ofstream(filename_prefix + L"_real_image.csv",ios::out); //파일을 연다, 없으면 생성
			for(i=0;i<my_total_point;i++) outFile<<var_str<<i*2+1<<","<<var_str<<i*2+2<<",";

			// TEMP 160517 
			if(mode==IDX_DATA_S22)
			{
				for(i=my_total_point;i<=my_total_point;i++) outFile<<var_str<<i*2+1<<",";
			}

			outFile<<endl;
			for(i=0;i<my_total_point;i++) outFile<<present_data[mode][IDX2_REAL][i]<<','<<present_data[mode][IDX2_IMAGE][i]<<',';

			outFile.close();

			outFile = ofstream(filename_prefix +L"_real.csv",ios::out); //파일을 연다, 없으면 생성
			for(i=0;i<my_total_point;i++) outFile << var_str << i+1<<',';
			outFile<<endl;
			for(i=0;i<my_total_point;i++) outFile<<present_data[mode][IDX2_REAL][i]<<',';
			outFile.close();

			outFile = ofstream(filename_prefix + L"_image.csv",ios::out); //파일을 연다, 없으면 생성
			for(i=0;i<my_total_point;i++) outFile << var_str << i+1<<',';
			outFile<<endl;
			for(i=0;i<my_total_point;i++) outFile<<present_data[mode][IDX2_IMAGE][i]<<',';
			outFile.close();

			outFile = ofstream(filename_prefix + L"_mag.csv",ios::out); //파일을 연다, 없으면 생성
			for(i=0;i<my_total_point;i++) outFile << var_str << i+1<<',';
			outFile<<endl;
			for(i=0;i<my_total_point;i++) outFile<<present_data[mode][IDX2_MAG][i]<<',';
			outFile.close();


			outFile = ofstream(filename_prefix + L"_phase.csv",ios::out); //파일을 연다, 없으면 생성
			for(i=0;i<my_total_point;i++) outFile << var_str << i+1<<',';
			outFile<<endl;
			for(i=0;i<my_total_point;i++) outFile<<present_data[mode][IDX2_PHASE][i]<<',';
			outFile.close(); 
		
			data_sqc[mode] = data_fetch_sqc;
			data_cnt[mode] = my_total_point;
		}
		// input_touched.txt 를 생성

		filename_prefix.Format(
			L"C:\\filter_tuning_program\\legacy\\",
			project_name);


		outFile = ofstream(filename_prefix + L"input_touched.txt",ios::out); //파일을 연다, 없으면 생성
		outFile<<"baegopa";
		outFile.close(); 
	}
	return ANDY_YES;
#else
	return ANDY_YES;
#endif
}

void CNetworkAnalyzerCtr::close()
{
#ifdef USE_VISA
	if(_is_opened)
	{
		viClose(vi);
	}
#endif
}
 

void CNetworkAnalyzerCtr::disp_off(ViSession* vi)
{
	//SYSTEM:DISP:UPDOFF
}
void CNetworkAnalyzerCtr::disp_on(ViSession* vi)
{

}
void CNetworkAnalyzerCtr::initialize_model_builder()
{
	if(OPERATION_MODE==1) return;
	int i,j,k;
#ifdef USE_VISA
	//#define Rohde_AND_Schwarz 1
	//#define AgilentENA 2
	//#define AgilentPNA 3
	//#define AgilentFieldFox 4

	EnterCriticalSection(&cs);
	if(Typenetworkanal==Rohde_AND_Schwarz)
	{
		initialize_model_builderRohde_AND_Schwarz();
	}
	else if(Typenetworkanal==AgilentENA)
	{
		initialize_model_builderENA();
	}
	LeaveCriticalSection(&cs);
#endif
}


void CNetworkAnalyzerCtr::initialize_model_builderRohde_AND_Schwarz()
{
	if(OPERATION_MODE==1) return;
	int i,j,k;
#ifdef USE_VISA
	ViPUInt32 pp;
	char buf [1024] = {0};
	pp = (ViPUInt32)malloc(sizeof(ViPUInt32));
	//	mywrite("SYSTEM:DISP:UPD OFF", pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CONF:TRAC:CAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CONF:TRAC:CHAN:NAME:ID? \"MyMeas_1_1_S11\"",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CALC1:PAR:DEL \"MyMeas_1_1_S11\"",pp);

	mywrite("CONF:TRAC:CHAN:NAME:ID? \"MyMeas_1_2_S22\"",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CALC1:PAR:DEL \"MyMeas_1_2_S22\"",pp);

	mywrite("CONF:TRAC:CHAN:NAME:ID? \"MyMeas_1_3_S21\"",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CALC1:PAR:DEL \"MyMeas_1_3_S21\"",pp);

	mywrite("CONF:CHAN2 OFF",pp);

	mywrite("DISP:WIND1:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND1:STAT OFF",pp);

	mywrite("DISP:WIND2:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("DISP:WIND3:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("DISP:WIND4:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("DISP:WIND5:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("DISP:WIND6:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND7:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND8:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND9:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND10:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND11:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND12:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND13:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND14:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND15:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("DISP:WIND16:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":FORM:DATA ASC,0",pp);

	mywrite("DISP:WIND1:STAT ON",pp);

	// for(i=0;i<present_filter_data.option_data.aTraceOrder;)

	std::list<CString>::iterator itor;
	i = 0;

	char measurement_cmd[1024];
	char feed_cmd[1024];
	char order_in_char[1024];
	int nOrderCount = 0;
	for(itor=project->option_data.aTraceOrderString.begin();
		itor!=project->option_data.aTraceOrderString.end();
		itor++)
	{
		i++;
		nOrderCount++;
		CString ttpp = (*itor);
		strcpy(order_in_char, CT2A(ttpp));


		sprintf(measurement_cmd, "CALC1:PAR:SDEF \"MyMeas_1_%d_%s\",\"%s\"",i,order_in_char,order_in_char);
		mywrite(measurement_cmd,pp);

		mywrite("DISP:WIND1:STAT ON",pp);


		sprintf(feed_cmd, "DISP:WIND1:TRAC%d:FEED \"MyMeas_1_%d_%s\"",i,i,order_in_char);
		mywrite(feed_cmd,pp);
	} 


	mywrite(":CALC1:FORM MLOG",pp);
	mywrite("SENS1:SEGM:DEL:ALL",pp);
	mywrite("SENS1:SEGM:BWID:CONT ON",pp);
	mywrite("SENS1:SEGM:POW:CONT ON",pp);
	int freq_add = 0;

	for(i=0;i<TOTAL_SWEEP_RANGE;i++)
	{
		char dstr[1024];
		bool _is_final_range = false;
		SFilterSweepItem myitem = project->item_data[i];
		if(i==TOTAL_SWEEP_RANGE-1 || project->item_data[i+1].nExist==0)
		{
			_is_final_range = 1;
		}
		if(myitem.nExist==0)
		{
			break;
		}

		sprintf(dstr,"SENS1:SEGM%d:ADD", i+1);
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:BWID %d", i+1,myitem.nInputFilter); // input filter
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:FREQ:STAR %I64d", i+1,myitem.nStartFreq);
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:FREQ:STOP %I64d", i+1,myitem.nStopFreq);
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:POW %d", i+1,myitem.nPower); // power
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:SWE:POIN %d", i+1,myitem.nPoint); // point
		mywrite(dstr,pp);
		sprintf(dstr,"SENS1:SEGM%d:ON", i+1);
		mywrite(dstr,pp);

		double dEndFreq;
		double dStartFreq;
		double dPoint;
		dEndFreq = myitem.nStopFreq;
		dStartFreq = myitem.nStartFreq;
		dPoint = myitem.nPoint;

		if(dPoint>1 && _is_final_range) 
		{
			dPoint = myitem.nPoint - 1;
		}

		for(j=0;j<myitem.nPoint;j++)
		{
			for(k=0;k<128;k++)
			{
				present_data[k][IDX2_FREQ][freq_add] = (double)j / dPoint * (dEndFreq - dStartFreq) + dStartFreq; 
			}
			freq_add++;
		}
	}

	mywrite("SENS1:SWE:TYPE SEGM",pp);
	mywrite(":INIT1:CONT ON",pp);
	mywrite(":INIT:SCOP ALL",pp);
	mywrite(":INIT:CONT OFF",pp);
	mywrite(":STAT:OPER:PTR 0",pp);
	mywrite(":STAT:OPER:NTR 16",pp);
	mywrite(":STAT:OPER:ENAB 16",pp);
	mywrite("*SRE 128",pp);
	mywrite("*CLS",pp);
	mywrite(":INIT:IMM",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":INIT:IMM",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":INIT:CONT ON",pp);
	mywrite(":INIT1:CONT ON",pp);
	mywrite(":INIT1:CONT ON",pp);

	mywrite(":DISP:WIND1:TRAC1:Y:AUTO ONCE",pp);
	mywrite(":DISP:WIND1:TRAC2:Y:AUTO ONCE",pp);
	mywrite(":DISP:WIND1:TRAC3:Y:AUTO ONCE",pp);

	mywrite(":SENS1:AVER OFF",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);


	mywrite("CALC1:PAR1:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR2:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR3:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);


	mywrite("SYSTEM:DISP:UPD ON",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR:SEL \"MyMeas_1_1_S11\"",pp);
	mywrite("CALC1:LIM:DEL:ALL",pp);
	mywrite("CALC1:LIM:DISP OFF",pp);
	mywrite("CALC1:PAR:SEL \"MyMeas_1_3_S21\"",pp);
	mywrite("CALC1:LIM:DEL:ALL",pp);
	mywrite("CALC1:LIM:DISP OFF",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CALC1:PAR:SEL \"MyMeas_1_1_S11\"",pp);
	mywrite("CALC1:RIPP:DEL:ALL",pp);
	mywrite("CALC1:RIPP:DISP OFF",pp);
	mywrite("CALC1:PAR:SEL \"MyMeas_1_3_S21\"",pp);
	mywrite("CALC1:RIPP:DEL:ALL",pp);
	mywrite("CALC1:RIPP:DISP OFF",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("CALC1:PAR:SEL \"MyMeas_1_1_S11\"",pp);
	mywrite("CALC1:LIM:DEL:ALL",pp);
	mywrite("CALC1:LIM:DISP OFF",pp);

	mywrite("CALC1:PAR:SEL \"MyMeas_1_3_S21\"",pp);
	mywrite("CALC1:LIM:DEL:ALL",pp);
	mywrite("CALC1:LIM:DISP OFF",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":SENS1:CORR:STAT?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite("SYSTEM:DISP:UPD OFF",pp);
	mywrite("*OPC?",pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	free(pp);
#endif
}

void CNetworkAnalyzerCtr::initialize_model_builderENA()
{
	if(OPERATION_MODE==1) return;
	int i,j,k;
#ifdef USE_VISA
	ViPUInt32 pp;
	char buf [1024] = {0};
	pp = (ViPUInt32)malloc(sizeof(ViPUInt32));
	mywrite(":DISP:ENAB OFF", pp);

	
	i = 0;

	char measurement_cmd[1024];
	char feed_cmd[1024];
	char order_in_char[1024];
	
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":DISP:SPL D12",pp);
	//viRead (vi, (ViBuf)buf, 1000, pp);
	mywrite(":DISP:WIND2:ACT",pp);
	mywrite(":INIT1:CONT OFF",pp);


	mywrite("TRIG:SEQ:SCOP ALL",pp);
	mywrite("INIT:CONT OFF",pp);

	mywrite("TRIG:SEQ:SCOP ACT",pp);
	mywrite(":FORM:DATA ASC",pp);
	mywrite(":DISP:SPL D1",pp);
	mywrite(":DISP:WIND1:ACT",pp);
	mywrite(":CALC1:PAR:COUN 3",pp);
	mywrite(":CALC1:FORM MLOG",pp);




	
	//char* dstr= new char[1024];
	//char* dstr1= new char[1024];
	char dstr[1024];
	char dstr1[10240];

	//memset(dstr,'a',sizeof(dstr));
	//memset(dstr1,NULL,sizeof(dstr1));

	
	
	temptotalcount = 0;
	sprintf(dstr,"");
	for(i=0;i<TOTAL_SWEEP_RANGE;i++)
	{
		bool _is_final_range = false;
		SFilterSweepItem myitem = project->item_data[i];
		if(i==TOTAL_SWEEP_RANGE-1 || project->item_data[i+1].nExist==0)
		{
			_is_final_range = 1;
		}
		if(myitem.nExist==0)
		{
			break;
		}
		char dstrtemp[512];
		//memset(dstrtemp,'a',sizeof(dstrtemp));
		temptotalcount++;
		int kkk = myitem.nStopFreq;
		sprintf(dstrtemp,",%d",myitem.nStartFreq);
		strcat(dstr,dstrtemp);
		sprintf(dstrtemp,",%d",myitem.nStopFreq);
		strcat(dstr,dstrtemp);
		sprintf(dstrtemp,",%d",myitem.nPoint);
		strcat(dstr,dstrtemp);
		sprintf(dstrtemp,",%d",myitem.nInputFilter);
		strcat(dstr,dstrtemp);

	}
	sprintf(dstr1,"SENS1:SEGM:DATA 5,0,1,0,0,0,%d%s",temptotalcount,dstr);

	mywrite(dstr1,pp);
	nOrderCount1 =0 ;
	std::list<CString>::iterator itor;
	for(itor=project->option_data.aTraceOrderString.begin();
		itor!=project->option_data.aTraceOrderString.end();
		itor++)
	{
		i++;
		nOrderCount1++;
		CString ttpp = (*itor);
		strcpy(order_in_char, CT2A(ttpp));


		sprintf(measurement_cmd, ":CALC1:PAR%d:DEF %s",i,order_in_char);
		mywrite(measurement_cmd,pp);

		mywrite("DISP:WIND1:STAT ON",pp);
	} 
	mywrite("SENS1:SWE:TYPE SEGM",pp);
	mywrite(":INIT1:CONT ON",pp);
	mywrite(":TRIG:SOUR BUS",pp);
	mywrite(":TRIG:SING",pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);


	mywrite(":DISP:UPD",pp);
	mywrite(":TRIG:SOUR INT",pp);
	mywrite(":INIT1:CONT ON",pp);
	mywrite(":INIT1:CONT ON",pp);

	i =0;
	nOrderCount1  =0;
	for(itor=project->option_data.aTraceOrderString.begin();
		itor!=project->option_data.aTraceOrderString.end();
		itor++)
	{
		i++;
		nOrderCount1 = nOrderCount1+1;
		CString ttpp = (*itor);
		strcpy(order_in_char, CT2A(ttpp));
		sprintf(measurement_cmd, ":CALC1:PAR%d:DEF",i,order_in_char);
		mywrite(measurement_cmd,pp);
	} 



	mywrite(":SENS1:AVER OFF",pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR1:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR2:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("CALC1:PAR3:SEL",pp);
	mywrite("CALC1:SMO:STAT OFF",pp);
	mywrite("CALC1:SMO:APER 1.5",pp);
	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);


	mywrite(":DISP:UPD",pp);
	mywrite("CALC1:PAR1:SEL",pp);
	mywrite("CALC1:SEL:LIM:DATA 0",pp);
	mywrite("CALC1:SEL:LIM:DISP OFF",pp);

	mywrite("CALC1:PAR3:SEL",pp);
	mywrite("CALC1:SEL:LIM:DATA 0",pp);
	mywrite("CALC1:SEL:LIM:DISP OFF",pp);


	mywrite("CALC1:PAR1:SEL",pp);
	mywrite("CALC1:SEL:RLIM:DATA 0",pp);
	mywrite("CALC1:SEL:RLIM:DISP:LINE OFF",pp);

	mywrite("CALC1:PAR3:SEL",pp);
	mywrite("CALC1:SEL:RLIM:DATA 0",pp);
	mywrite("CALC1:SEL:RLIM:DISP:LINE OFF",pp);




	mywrite("*OPC?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite(":SENS1:CORR:STAT?", pp);
	viRead (vi, (ViBuf)buf, 1000, pp);

	mywrite("DISP:WIND1:STAT ON",pp);



	/////////////// 끝!!!!!!!!!!!!!!!!!!!!!!!!///////////////
	//delete[] dstr;
	//delete[] dstr1;
	free(pp);
	//OPERATION_MODE=1;
	//delete dstr;
	//delete dstr1;
#else

#endif
}



void CNetworkAnalyzerCtr::mywrite(const char* mystr, ViPUInt32 retCnt)
{
#ifdef USE_VISA
	viWrite (vi, (ViBuf)mystr, strlen(mystr), retCnt);
#endif
}
void CNetworkAnalyzerCtr::fetch()
{
#ifdef USE_VISA

	if(!_is_opened)
	{
		open(); 
	}

	char buf [1024] = {0};


	viClear (vi);

	ViPUInt32 pp;
	ViPUInt16 pp16;
	ViPAddr voidpp;

	pp = (ViPUInt32)malloc(sizeof(ViPUInt32));
	pp16 = (ViPUInt16)malloc(sizeof(ViPUInt16));
	voidpp = (ViPAddr)malloc(sizeof(ViPAddr));

	viReadSTB (vi, pp16);


	viWrite(vi, (ViBuf)"SYST:ERR?", 9, pp);

	viRead (vi, (ViBuf)buf, 1000, pp);
	viSetAttribute(vi, VI_ATTR_TMO_VALUE, 5000);
	viWrite (vi, (ViBuf)"*IDN?", 5, pp);

	viRead (vi, (ViBuf)buf, 1000, pp);

	// viWrite (vi, (ViBuf)":FORMat:DATA REAL", 17, pp);
	mywrite(":FORMat:DATA REAL",pp);
	// viWrite (vi, (ViBuf)":INSTrument:PORT:COUNt?", 23, pp);
	mywrite(":INSTrument:PORT:COUNt?",pp);

	viRead (vi, (ViBuf)buf, 1000, pp);


	viWrite (vi, (ViBuf)":SENSe1:FUNCtion:ON 'XFRequency:POWer:S11", 41, pp);
	viWrite (vi, (ViBuf)":CALCulate1:FORMat MLOGarithmic", 31, pp);
	viWrite (vi, (ViBuf)":INITiate1:CONTinuous OFF", 25, pp);
	viWrite (vi, (ViBuf)":INITiate1:IMMediate;*WAI", 25, pp);
	viWrite (vi, (ViBuf)"TRAC:STIM? CH1DATA", 18, pp);
	viGetAttribute (vi, VI_ATTR_GPIB_READDR_EN, voidpp); // GPIB_READDR_EN-> VI_ATTR_GPIB_READDR_EN
	viSetAttribute (vi, VI_ATTR_GPIB_READDR_EN, VI_FALSE);
	viRead (vi, (ViBuf)buf, 2, pp);
	viRead (vi, (ViBuf)buf, 4, pp); 
	viReadToFile (vi, "C:\\mywork\\tmp\\abc111.tmp", 1250, pp);

	int i,ptr=0;

	viSetAttribute (vi, VI_ATTR_GPIB_READDR_EN, VI_FALSE);
	viWrite (vi, (ViBuf)":TRACe? CH1Data", 15, pp);
	viGetAttribute (vi, VI_ATTR_GPIB_READDR_EN, voidpp);
	viSetAttribute (vi, VI_ATTR_GPIB_READDR_EN, VI_FALSE);
	viRead (vi, (ViBuf)buf, 2, pp);
	viRead (vi, (ViBuf)buf, 4, pp);
	viReadToFile (vi, "C:\\mywork\\tmp\\abc222.tmp", 2490, pp);
	//	viRead (vi, (ViBuf)buf, 2490, pp);

	viSetAttribute (vi, VI_ATTR_GPIB_READDR_EN, VI_FALSE);
	viWrite (vi, (ViBuf)":INITiate1:CONTinuous ON", 24, pp);

	//viClose(vi);


	ifstream is;
	is.open ("C:\\mywork\\tmp\\abc111.tmp", ios::binary );
	is.seekg (0, ios::end);
	int length = is.tellg();
	char* buffer = new char [length];
	is.seekg (0, ios::beg);
	is.read (buffer,length);
	for(i=0;i<1240;i+=4)
	{
		memcpy(&data_freq[count_freq++],&buffer[i],sizeof(double));
	}
	delete buffer;
	is.close();


	is.open ("C:\\mywork\\tmp\\abc222.tmp", ios::binary );
	is.seekg (0, ios::end);
	length = is.tellg();
	buffer = new char [length];
	is.seekg (0, ios::beg);
	is.read (buffer,length);
	ptr = 0;

	for(i=0;i<1240*2;i+=8)
	{
		memcpy(&data_real[count_data],&buffer[i],sizeof(double));
		memcpy(&data_image[count_data],&buffer[i+4],sizeof(double));
		count_data++; 
	}
	// B: real, C: image
	for(i=0;i<count_data;i++)
	{
		data_mag[i] = - 20.0 * log(sqrt(pow(data_real[i],2)+pow(data_image[i],2))) / log(10.0);

		data_phase[i] = 180/M_PI*atan2(data_image[i], data_real[i]);
		if(data_phase[i]>180) data_phase[i] -= 360;

/*		if(data_real[i]>0) data_phase[i] = 180/M_PI*atan2(data_image[i], data_real[i]);
		else if(data_image[i]>0)  data_phase[i] = 180.0+180.0/M_PI*atan2(data_image[i], data_real[i]);
		else data_phase[i] = 180.0/M_PI*atan2(data_image[i], data_real[i])-180.0;
		*/
		//
		// Mag [dB]: =20*LOG(SQRT(B4^2+C4^2))
		// Phase [°]: =IF(B4>0,180/PI()*ATAN(C4/B4),IF(C4>0,180+180/PI()*ATAN(C4/B4),180/PI()*ATAN(C4/B4)-180))
	}
	is.close();

	char* var_str ="VAR";

	ofstream outFile;
	outFile = ofstream(L"c:\\mywork\\data\\input_real_image.csv",ios::out); //파일을 연다, 없으면 생성
	for(i=0;i<count_data;i++) outFile<<var_str<<i*2+1<<","<<var_str<<i*2+2<<",";
	outFile<<endl;
	for(i=0;i<count_data;i++) outFile<<data_real[i]<<','<<data_image[i]<<',';
	outFile.close();

	outFile = ofstream(L"c:\\mywork\\data\\input_real.csv",ios::out); //파일을 연다, 없으면 생성
	for(i=0;i<count_data;i++) outFile << var_str << i+1<<',';
	outFile<<endl;
	for(i=0;i<count_data;i++) outFile<<data_real[i]<<',';
	outFile.close();

	outFile = ofstream(L"c:\\mywork\\data\\input_image.csv",ios::out); //파일을 연다, 없으면 생성
	for(i=0;i<count_data;i++) outFile << var_str << i+1<<',';
	outFile<<endl;
	for(i=0;i<count_data;i++) outFile<<data_image[i]<<',';
	outFile.close();

	outFile = ofstream(L"c:\\mywork\\data\\input_mag.csv",ios::out); //파일을 연다, 없으면 생성
	for(i=0;i<count_data;i++) outFile << var_str << i+1<<',';
	outFile<<endl;
	for(i=0;i<count_data;i++) outFile<<data_mag[i]<<',';
	outFile.close();


	outFile = ofstream(L"c:\\mywork\\data\\input_phase.csv",ios::out); //파일을 연다, 없으면 생성
	for(i=0;i<count_data;i++) outFile << var_str << i+1<<',';
	outFile<<endl;
	for(i=0;i<count_data;i++) outFile<<data_phase[i]<<',';
	outFile.close();


	free(pp);
	free(pp16);
	free(voidpp);
#endif
}





std::list<int> CNetworkAnalyzerCtr::GetTraceOrder()
{
	return project->GetTraceOrder();
}

int CNetworkAnalyzerCtr::createDirectoryRecursively( LPCTSTR path )
{
	return SHCreateDirectoryEx( NULL, path, NULL );
}

/*
void CNetworkAnalyzerCtr::CopyPresentDataTo(int& count, boolean* data_exist, double (*mymy)[5][2048])
{
	int i,j,k;
	for(i=0;i<128;i++)
	{
		if(present_data[i]==false)
		{
			continue;
		}
		for(j=0;j<5;j++)
		{
			for(k=0;k<present_data_count;k++)
			{
				mymy[i][j][k] = present_data[i][j][k];
			}
		}
	}
	count = this->present_data_count;

}
void CNetworkAnalyzerCtr::CopyPresentDataTo( int& count, int idx, double (*mymy)[2048] )
{
	if(idx<0) return;
#ifdef USE_VISA
	int i,itor_idx2;
	if(present_data[idx]==false)
	{
		count = 0;
		return;
	}
	
	count = this->present_data_count;
	for(itor_idx2=0;itor_idx2<5;itor_idx2++)
	{
		for(i=0;i<count;i++)
		{
			mymy[itor_idx2][i] = present_data[idx][itor_idx2][i];
		}
	}
#else
	count = 0;
#endif
} 
*/
void CProject::SetUsingInternalDevice( bool _is_using_internal )
{
	_using_internal_device_flag = _is_using_internal;
}
