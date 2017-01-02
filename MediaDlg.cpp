
// MediaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Media.h"
#include "MediaDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CMediaDlg dialog




CMediaDlg::CMediaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMediaDlg::IDD, pParent)
	, m_RadioLang(0)
	, m_RadioDS(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeInfo);
	DDX_Radio(pDX, IDC_RADIO_CPP, (int &)m_RadioLang);
	DDX_Radio(pDX, IDC_RADIO_DS_INTERNAL, (int &)m_RadioDS);
}

BEGIN_MESSAGE_MAP(CMediaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	
	ON_MESSAGE(WM_TRAYICON, OnTrayIcon)
	ON_COMMAND(WM_APP_EXIT, OnAppExit)
	ON_COMMAND(WM_DIALOG_SHOW, OnDialogShow)


	ON_BN_CLICKED(IDC_BUTTON3, &CMediaDlg::OnBnClickedButtonXmlInfo)
	ON_BN_CLICKED(IDC_BUTTON4, &CMediaDlg::OnBnClickedButtonXmlInfoProjectList)
	ON_BN_CLICKED(IDC_BUTTON5, &CMediaDlg::OnBnClickedButtonXmlInputDataSet)
	ON_BN_CLICKED(IDC_BUTTON7, &CMediaDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMediaDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON_FT_LEARN, &CMediaDlg::OnBnClickedButtonFTLearn)
	ON_BN_CLICKED(IDC_BUTTON11, &CMediaDlg::OnBnClickedButtonFTTest)
	ON_BN_CLICKED(IDOK, &CMediaDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_PREDICTION, &CMediaDlg::OnBnClickedButtonShowPrediction)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_COLLECTION, &CMediaDlg::OnBnClickedButtonShowCollection)
	ON_BN_CLICKED(IDC_BUTTON13, &CMediaDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CMediaDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CMediaDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CMediaDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CMediaDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON_GO_START, &CMediaDlg::OnBnClickedButtonGoStart)
	ON_BN_CLICKED(IDC_BUTTON_CYCLE, &CMediaDlg::OnBnClickedButtonCycle)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CMediaDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON18, &CMediaDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON20, &CMediaDlg::OnBnClickedButton20)
	ON_WM_TIMER()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CPP, IDC_RADIO_RSCRIPT, CMediaDlg::SetRadioStatus)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_DS_INTERNAL, IDC_RADIO_DS_EXTERNAL, CMediaDlg::SetRadioStatus)

//	ON_BN_CLICKED(IDC_BUTTON_SHOW_PREDICTION2, &CMediaDlg::OnBnClickedButtonShowPrediction2)
	ON_BN_CLICKED(IDC_RADIO_CPP, &CMediaDlg::OnBnClickedRadioCpp)
	ON_BN_CLICKED(IDC_BUTTON_FT, &CMediaDlg::OnBnClickedButtonFt)
	ON_BN_CLICKED(IDC_BUTTON_FILTER_RT, &CMediaDlg::OnBnClickedButtonFilterFt)
	ON_BN_CLICKED(IDC_BUTTON_RT_LEARN, &CMediaDlg::OnBnClickedButtonTwLearn)
	ON_BN_CLICKED(IDC_BUTTON10, &CMediaDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON23, &CMediaDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON22, &CMediaDlg::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON1, &CMediaDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, &CMediaDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON9, &CMediaDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON191, &CMediaDlg::OnBnClickedButton191)
	ON_BN_CLICKED(IDC_BUTTON21, &CMediaDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON24, &CMediaDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON25, &CMediaDlg::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CMediaDlg::OnBnClickedButton26)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CMediaDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON2711, &CMediaDlg::OnBnClickedButton2711)
	ON_BN_CLICKED(IDC_RADIO_RSCRIPT, &CMediaDlg::OnBnClickedRadioRscript)
	ON_BN_CLICKED(IDC_BUTTON28, &CMediaDlg::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON29, &CMediaDlg::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON30, &CMediaDlg::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON31, &CMediaDlg::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CMediaDlg::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON33, &CMediaDlg::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON34, &CMediaDlg::OnBnClickedButton34)
END_MESSAGE_MAP()


// CMediaDlg message handlers

BOOL CMediaDlg::OnInitDialog()
{

	int iCnt = 0;
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW( GetCommandLine(), &iCnt);

	CString str2;
	for(int i=0; i<iCnt; i++)
	{
		CString str;
		str.Format(L"%s", pStr[i]);  //배열 처럼 쓸수있다. // pStr[0]은 실행파일. 1번부터가 인자
		if(i==1) str2 = str;
		// AfxMessageBox(str);
	}
	if(iCnt>=2 && str2==L"inv")
	{
		_invisible_flag = 1;
	}
	else
	{
		_invisible_flag = 0;
	}
	LocalFree(pStr);






	CDialogEx::OnInitDialog();
	dlgPrediction = new CPredictionDlg();
	dlgPrediction->Create(IDD_PREDICTION_DIALOG);
	//dlgPrediction->ShowWindow(SW_SHOW);

	dlgCollection = NULL;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ctr = new CNetworkAnalyzerCtr(); // 계측기 제어
	handler = new CMediaHandler(); // 중개(외부 통신, 파이프, 소켓, xml 파일 생성 등)
	handler->start();

	project = new CProject(); // 프로젝트 관리
	

	std::list<CString> pp;
	pp = project->GetProjectNameList();
//	CString project_name, filter_name, sw_name;
	wchar_t input[1024];

	/*
	project->LoadProjectAndFilterTuningProgramByINI();
	handler->LoadMasterWaveForm();
	*/

	hRoot = m_treeInfo.InsertItem(L"Machine", 0/* nImage */, 1/* nSelectedImage */, TVI_ROOT, TVI_LAST ) ;
	hChild[0] = m_treeInfo.InsertItem(L"project name", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST ) ;
	hChildProjectName = hChild2[0] = m_treeInfo.InsertItem(project->GetProjectName(), 1/* nImage */, 1/* nSelectedImage */, hChild[0], TVI_LAST ) ;

	hChild[1] = m_treeInfo.InsertItem(L"filter name", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST ) ;
	hChildFilterName = hChild2[1] = m_treeInfo.InsertItem(project->GetFilterName(), 1/* nImage */, 1/* nSelectedImage */, hChild[1], TVI_LAST ) ;

	hChild[2] = m_treeInfo.InsertItem(L"sw name", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST ) ;
	hChildSwName = hChild2[2] = m_treeInfo.InsertItem(project->GetSWName(), 1/* nImage */, 1/* nSelectedImage */, hChild[2], TVI_LAST ) ;

	hChild[3] = m_treeInfo.InsertItem(L"info", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST ) ;
	hChildFetchCount = hChild2[3] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildMachStatus = hChild2[4] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildPingCount = hChild2[5] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildPoleOfInterest = hChild2[6] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildMsgCountFromServerCount = hChild2[7] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate0 = hChild2[8] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate1 = hChild2[9] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate2 = hChild2[10] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate3 = hChild2[11] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate4 = hChild2[12] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildErrorRate5 = hChild2[13] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildTotalPoints = hChild2[14] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildWaveRefl = hChild2[15] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;
	hChildWaveTran = hChild2[16] = m_treeInfo.InsertItem(L"", 1/* nImage */, 1/* nSelectedImage */, hChild[3], TVI_LAST ) ;

	int i;
	for(i=0;i<4;i++)
	{
		m_treeInfo.Expand(hChild[i], TVE_EXPAND);
	}

	for(i=0;i<17;i++)
	{
		m_treeInfo.Expand(hChild2[i], TVE_EXPAND);
	}
	m_treeInfo.Expand(hRoot, TVE_EXPAND);
	

	SetTimer(0,100,NULL);
	SetDlgItemText(IDC_EDIT_XML_LOC,L"C:\\dev_folder\\test.xml");


	m_myTray.m_bHide = _invisible_flag;
	m_myTray.AddTrayIcon(GetSafeHwnd());	//시작과 동시에 트레이 아이콘 표시



	 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMediaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMediaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMediaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





tinyxml2::XMLError CMediaDlg::test_xml1()
{

	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Tiny Xml Lib/resources/dream.xml" );

	return doc.ErrorID();

}

template< class T > bool CMediaDlg::XMLTest( const char* testString, T expected, T found, bool echo )
{
	bool pass = ( expected == found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");
/*
	if ( !echo )
		printf (" %s\n", testString);
	else
		printf (" %s [%d][%d]\n", testString, static_cast<int>(expected), static_cast<int>(found) );
		*/
	if ( pass )
		++gPass;
	else
		++gFail;
	return pass;
}

void CMediaDlg::OnBnClickedButtonXmlInfo()
{  
}


void CMediaDlg::OnBnClickedButtonXmlInfoProjectList()
{
	// TODO: Add your control notification handler code here
	char output[1024];
	int len = handler->GenerateInfoProjectListXML(output,1024);
	len = len;
}


void CMediaDlg::OnBnClickedButtonXmlInputDataSet()
{
	// TODO: Add your control notification handler code here
	char output[1024];
	int len = handler->GenerateInputDataSetXML(NULL,output,1024);
	len = len;
}



void CMediaDlg::OnBnClickedButton7()
{
	// sample_data\160721
	// TODO: Add your control notification handler code here
	handler->LoadWaveFormFromXMLFile(L"./sample_data/160721/input_data_set(일반).xml");
}




void CMediaDlg::OnBnClickedButtonFTLearn()
{
	const double R_SCALE_MAX = 250;
	const double C_SCALE_MAX = 125;


	//seed random number generator
	int num_input	= 530;
	int num_hidden	= 50;
	int num_output	=	project->GetTotalPoles();
	int num_poles   =	project->GetTotalPoles();
	int num_train	= 1211;
	double learning_rate = 0.1;
	double maxs[MAX_NO_POLES];
	double mins[MAX_NO_POLES];

	double **test_input		= new double*[num_train];
	double **input			= new double*[num_train];
	double **target_output	= new double*[num_train];

	//Rprop *rProp  = new Rprop(num_input, num_hidden, num_output);


	for(int i=0;i<num_poles;i++)
	{
		if(project->GetPolePropertyAt(i+1)==L"R")		{ maxs[i] = R_SCALE_MAX; mins[i] = -R_SCALE_MAX; }
		else if(project->GetPolePropertyAt(i+1)==L"C")	{ maxs[i] = C_SCALE_MAX; mins[i] = -C_SCALE_MAX; }
		else { ASSERT(0); OutputDebugString(L"CMediaHandler::LearnRT\r\nR과 C외에는 준비되지 않았습니다"); }
	} 


	Feedforward_Neural_Network *FNN = new Feedforward_Neural_Network(num_input, num_hidden, num_output);

	for(int i = 0;i < num_train;i++){
		test_input[i]		= new double[num_input];
		input[i]			= new double[num_input];
		target_output[i]	= new double[num_output];
	}

	// 셔플된 1211개의 트레이닝 데이터를 스케일링(2.5배) 한 입력데이터
	fstream inputFile;
	inputFile.open("scaled_largedata.csv", ios::in);	

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


	int i, j;
	for(i=1;i<=num_poles; i++)
	{
		int index_count = 0;
		double **input_part			= new double*[11];
		double **output_part		= new double*[11];
		double *output_ref			= new double[19];

		for(j=(10*(i-1))+(i-1); j<= (10*i)+(i-1); j++)
		{
				output_part[index_count]			= target_output[j];
				input_part[index_count++]			= input[j];
		}
		
		FNN->Train(5000, 11, learning_rate, input_part, output_part);	
		FNN->Test(input[0], output_ref);

	
		if(i<num_poles){ 
			for(j=(10*(i))+i; j<= (10*(i+1))+i; j++)
			{
				for(int k =0; k< num_poles; k++)
				{
					target_output[j][k] = target_output[j][k] + (output_ref[k]-0.5);
				}					
			}
		}

		delete[] input_part;
		delete[] output_part;
		delete output_ref;
	}

	//트레이닝

	FNN->Train(70000, num_train, learning_rate, input, target_output);


	char fileloc[1024];
	CopyCStringToChar( project->GetWeightFTFileLoc(), fileloc );		
	
	FNN->saveWeight(fileloc);

	
	for(int i = 0;i < num_train;i++){
		delete[] input[i];
		delete[] target_output[i];
	}
	delete[] input;
	delete[] target_output;
	
	delete FNN;


	
}


//RT 러닝 추가 2016-08-03 나원준
void CMediaDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	// ~~~ 



	
}


void CMediaDlg::OnBnClickedButtonFTTest()
{	
	handler->StopLearning();
}


void CMediaDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

 


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	project->GetTotalPoints();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CMediaDlg::OnBnClickedButtonShowPrediction()
{
	// TODO: Add your control notification handler code here
	dlgPrediction->ShowWindow(SW_SHOW);
}


void CMediaDlg::OnBnClickedButtonShowPrediction2()
{
	// TODO: Add your control notification handler code here
	dlgPrediction->ShowWindow(SW_SHOW);
}

void CMediaDlg::OnBnClickedButtonShowCollection()
{
	// TODO: Add your control notification handler code here
	if(dlgCollection==NULL)
	{
		dlgCollection = new CCollectionDlg();
		dlgCollection->Create(IDD_COLLECTION_DIALOG);

	}
	dlgCollection->ShowWindow(SW_SHOW);
}


void CMediaDlg::OnBnClickedButton13()
{
	// TODO: Add your control notification handler code here
	handler->DoMeasurement(); 
	CString loc;
	//loc.Replace(L"\\",L"\\\\");
	GetDlgItemText(IDC_EDIT_XML_LOC,loc);
	char loc_char[1024];
	CopyCStringToChar(loc,loc_char);
	int len = handler->GenerateInputDataSetXML(loc_char, NULL,1024);

}


void CMediaDlg::OnBnClickedButton14()
{
	handler->StartCollecting();
	// handler->SetCollectSampleDataSignalAtThisTime()
}


void CMediaDlg::OnBnClickedButton15()
{
	// TODO: Add your control notification handler code here
	present_xml_input = L"./sample_data/160727/pole17_wonjun_remote_step_1.xml";
}


void CMediaDlg::OnBnClickedButton16()
{
	// TODO: Add your control notification handler code here

	present_xml_input = L"./sample_data/160727/pole17_wonjun_remote_step_2.xml";
}


void CMediaDlg::OnBnClickedButton17()
{
	// TODO: Add your control notification handler code here

	present_xml_input = L"./sample_data/160727/pole17_wonjun_remote_step_3.xml";
}


void CMediaDlg::OnBnClickedButtonGoStart()
{
	// TODO: Add your control notification handler code here
	handler->StartPrediction();
}


void CMediaDlg::OnBnClickedButtonCycle()
{
	// TODO: Add your control notification handler code here
	handler->GoCyclePrediction();;
		
}


void CMediaDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here

	handler->StopPrediction();
}


void CMediaDlg::OnBnClickedButton18()
{
	// TODO: Add your control notification handler code here
	
	
	present_xml_input = L"./sample_data/160730/pole17_15_wonjun_remote_step_1.xml";

}


void CMediaDlg::OnBnClickedButton19()
{
	// TODO: Add your control notification handler code here
}


void CMediaDlg::OnBnClickedButton20()
{
	// TODO: Add your control notification handler code here
	// handler->DoMeasurementMaster();
	// handler->LoadMasterWaveForm();
}


void CMediaDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(m_RadioDS==21)
	{
		m_RadioDS = 1;
		UpdateData(false);
	}
	if(nIDEvent==0)
	{
//		hChildFilterName->SetDlgItemText(L"!23");
		CString dstr, status;
		dstr.Format(L"%s",project->GetProjectName());
		if(m_treeInfo.GetItemText(hChildProjectName)!=dstr) 
			m_treeInfo.SetItemText(hChildProjectName, dstr); 

		dstr.Format(L"%s",project->GetFilterName());
		if(m_treeInfo.GetItemText(hChildFilterName)!=dstr) 
			m_treeInfo.SetItemText(hChildFilterName, dstr); 

		dstr.Format(L"%s",project->GetSWName());
		if(m_treeInfo.GetItemText(hChildSwName)!=dstr) 
			m_treeInfo.SetItemText(hChildSwName, dstr); 

		dstr.Format(L"Fetch Count: %d",handler->GetFetchCount());
		if(m_treeInfo.GetItemText(hChildFetchCount)!=dstr) 
			m_treeInfo.SetItemText(hChildFetchCount, dstr); 


		handler->GetMachineStatus(status);
		dstr.Format(L"Status: %s",status);
		if(m_treeInfo.GetItemText(hChildMachStatus)!=dstr) 
			m_treeInfo.SetItemText(hChildMachStatus, dstr); 

		dstr.Format(L"Ping Count: %d",handler->GetPingCount());
		if(m_treeInfo.GetItemText(hChildPingCount)!=dstr) 
			m_treeInfo.SetItemText(hChildPingCount, dstr); 

		dstr.Format(L"Pole of interest(RT): %d",project->GetRTTargetPole());
		if(m_treeInfo.GetItemText(hChildPoleOfInterest)!=dstr) 
			m_treeInfo.SetItemText(hChildPoleOfInterest, dstr); 

		dstr.Format(L"Message Count From Server: %d",general_msg_id_increment);
		if(m_treeInfo.GetItemText(hChildMsgCountFromServerCount)!=dstr) 
			m_treeInfo.SetItemText(hChildMsgCountFromServerCount, dstr); 


		dstr.Format(L"Error Rate(0): %.4lf",handler->GetFinalErrorRate(0));
		if(m_treeInfo.GetItemText(hChildErrorRate0)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate0, dstr); 


		dstr.Format(L"Error Rate(1): %.4lf",handler->GetFinalErrorRate(1));
		if(m_treeInfo.GetItemText(hChildErrorRate1)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate1, dstr); 

		dstr.Format(L"Error Rate(2): %.4lf",handler->GetFinalErrorRate(2));
		if(m_treeInfo.GetItemText(hChildErrorRate2)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate2, dstr); 


		dstr.Format(L"Error Rate(3): %.4lf",handler->GetFinalErrorRate(3));
		if(m_treeInfo.GetItemText(hChildErrorRate3)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate3, dstr); 


		dstr.Format(L"Error Rate(4): %.4lf",handler->GetFinalErrorRate(4));
		if(m_treeInfo.GetItemText(hChildErrorRate4)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate4, dstr); 

		dstr.Format(L"Error Rate(5): %.4lf",handler->GetFinalErrorRate(5));
		if(m_treeInfo.GetItemText(hChildErrorRate5)!=dstr) 
			m_treeInfo.SetItemText(hChildErrorRate5, dstr); 

		dstr.Format(L"Total Points: %.1lf",project->GetTotalPoints());
		if(m_treeInfo.GetItemText(hChildTotalPoints)!=dstr) 
			m_treeInfo.SetItemText(hChildTotalPoints, dstr); 

		dstr.Format(L"Reflection: %s",handler->sFianlRefl);
		if(m_treeInfo.GetItemText(hChildWaveRefl)!=dstr) 
			m_treeInfo.SetItemText(hChildWaveRefl, dstr); 

		dstr.Format(L"Transmission: %s",handler->sFianlTran);
		if(m_treeInfo.GetItemText(hChildWaveTran)!=dstr) 
			m_treeInfo.SetItemText(hChildWaveTran, dstr); 

	}

	if(_invisible_flag)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_SHOW);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMediaDlg::SetRadioStatus(UINT ID) 
{

	UpdateData(TRUE);


	if(ID>= IDC_RADIO_CPP && ID<=IDC_RADIO_RSCRIPT)
	{
		if(handler->IsRunning())
		{
			((CButton*)GetDlgItem(ID))->SetCheck(0);
			((CButton*)GetDlgItem(IDC_RADIO_CPP + handler->GetPredictionMode()))->SetCheck(1);
			AfxMessageBox(L"기계 작동 중 언어 변경 금지");				
		}
		else if(ID==IDC_RADIO_CPP) handler->SetPredictionMode(0);
		else if(ID==IDC_RADIO_RSCRIPT) handler->SetPredictionMode(1);
		else
		{
			ASSERT(0);
			AfxMessageBox(L"변경 불가");
		}
	}
	if(ID>= IDC_RADIO_DS_INTERNAL && ID<=IDC_RADIO_DS_EXTERNAL)
	{
		if(handler->IsRunning())
		{
			((CButton*)GetDlgItem(ID))->SetCheck(0);
			((CButton*)GetDlgItem(IDC_RADIO_DS_INTERNAL + handler->GetPredictionMode()))->SetCheck(1);
			OutputDebugString(L"CMediaDlg::SetRadioStatus	기계 작동 중 Data Source 변경 금지");
		}
		else if(ID == IDC_RADIO_DS_INTERNAL) handler->SetDataSourceMode(0);
		else if(ID == IDC_RADIO_DS_FILE) handler->SetDataSourceMode(1);
		else if(ID == IDC_RADIO_DS_EXTERNAL) handler->SetDataSourceMode(2);
		else
		{
			ASSERT(0);
			OutputDebugString(L"CMediaDlg::SetRadioStatus	변경 불가"); 
		}
	}
}


void CMediaDlg::OnBnClickedRadioCpp()
{
	// TODO: Add your control notification handler code here
}


void CMediaDlg::OnBnClickedButtonFt()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter",L"LOW",L"FT");

}


void CMediaDlg::OnBnClickedButtonFilterFt()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter",L"LOW",L"RT");

}



void CMediaDlg::OnBnClickedButtonTwLearn()
{
	// TODO: Add your control notification handler code herec
	CString sstr;
	if(handler->GetMachineStatus(sstr)==MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING)
	{
		handler->SetMachineStatus(MACHINE_STATUS::_MACH_STATUS_BUILD_FILTER_MODE);
		handler->LearnRT_wo_rprop();
		handler->SetMachineStatus(MACHINE_STATUS::_MACH_STATUS_PROJECT_SETTING);
	}
}


void CMediaDlg::OnBnClickedButton10()
{ 
}

void CMediaDlg::OnBnClickedButton23()
{
	handler->LearnFT_with_rprop_HardCoding();
}


void CMediaDlg::OnBnClickedButton22()
{
	handler->LearnRT_with_rprop_HardCoding();
}



void CMediaDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	char output[1024*16];
	int len = handler->GenerateInfoProjectPropertyXML("c:\\abcde\\abc.xml", output,sizeof(output));
	len = len;

}


void CMediaDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	ChangeLegacyLearningData(L"roswin_filter",L"LOW",L"RT");
// 	ChangeLegacyLearningData(L"roswin_filter",L"LOW","FT");
// 	ChangeLegacyLearningData(L"roswin_filter",L"HIGH","RT");
// 	ChangeLegacyLearningData(L"roswin_filter",L"HIGH","FT");
}

int CMediaDlg::GetFindCharCount(CString parm_string, char parm_find_char) 
{ 
  int length = parm_string.GetLength(), find_count = 0; 

  for(int i = 0; i < length; i++){ 
      if(parm_string[i] == parm_find_char) find_count++; 
  } 

  return find_count; 
} 


void CMediaDlg::ChangeLegacyLearningData( CString project_name, CString filter_name, CString sw_name )
{
	CString basepath = L"C:\\filter_tuning_program\\project_data\\" + project_name + "\\filter\\"+  filter_name + "\\" + sw_name + "\\learning_data\\legacy_data\\";
	CString tpath = basepath + _T("*");
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

		if (!finder.IsDirectory())
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
	
	/*
	IDX2_MAG
	IDX2_FREQ
	*/
	double waveform[5][2048];
	for(std::list<CString>::iterator itor = mylist.begin();
		itor!=mylist.end();
		itor++)
	{
		CString filename;
		CString filename_wo_ext;
		filename = *itor;
		if(filename.Right(5)!=L".info")
		{
			continue;
		}
		filename_wo_ext = filename.Left(filename.GetLength()-5);

		int cnt=0;
		CString strTok, lastStr;
		while(AfxExtractSubString(strTok, filename_wo_ext, cnt, '_'))
		{
			cnt++;
			lastStr = strTok;
		}


		if(filename_wo_ext.Left(7)==L"S22_S21")
		{
			filename_wo_ext = filename_wo_ext;

		}
		else if(filename_wo_ext.Left(7)==L"S33_S31")
		{

		}
		else if(filename_wo_ext.Left(3)==L"S33")
		{

		}
		else if(filename_wo_ext.Left(3)==L"S22")
		{
			filename = filename;
		}
		



		CStdioFile tip_file;
		int target_idx, target_degree;
		if(tip_file.Open(basepath + filename, CFile::modeRead | CFile::typeText)){

			// 성공적으로 파일이 열렸으면 원하는 작업을 한다.
			CString display_str = L"";
			CString str;
			while(tip_file.ReadString(str)){ 
				str = str.Trim();
				if(str.Left(1)==L"#") continue;
				cnt = 0 ;
				display_str += str; 				
				int sepCount = GetFindCharCount(str, '^');  
				CString* temp = new CString[sepCount+1];	
				while(AfxExtractSubString(strTok, str, cnt, '^'))  temp[cnt++] = strTok;                    

				if(sepCount==530)
				{
					//필요한 배열에 옮기기(Input)
					if(target_idx<0 || target_degree<0)
					{
						AfxMessageBox(L"!@#");
					}
					int i;
					for(i=0;i*2<sepCount;i++)
					{
						waveform[IDX2_REAL ][i] = _ttof(temp[i*2]);
						waveform[IDX2_IMAGE][i] = _ttof(temp[i*2+1]);
						waveform[IDX2_FREQ][i] = 0;
						waveform[IDX2_MAG][i] = 0;
						waveform[IDX2_PHASE][i] = 0;
					}
					handler->GenerateInputDataSetXML("c:\\tmp\\160901.xml",NULL,NULL,265,waveform,2-1,2-1);
					target_idx = -1;
					target_degree = -1;
				}
				if(sepCount==19)
				{
					//필요한 배열에 옮기기(Output)
					target_idx = 0;
					target_degree = 0;
					for(int i=0;i<sepCount;i++)
					{
						CString pp;
						pp = temp[i].Trim();
						if(pp!=L"0")
						{
							target_idx = i;
							target_degree = _ttoi(temp[i]);
						}
					}
				}

				display_str += "\r\n"; 
			} 

			// 작업을 마친후에 파일을 닫는다.
			tip_file.Close();
		}

	}

}


void CMediaDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	handler->StartLearning();

}


void CMediaDlg::OnBnClickedButton191()
{
	// TODO: Add your control notification handler code here
	handler->StopCollecting();
}


void CMediaDlg::OnBnClickedButton21()
{
	// TODO: Add your control notification handler code here
	handler->SetCollectMasterDataSignalAtThisTime(5);

}


void CMediaDlg::OnBnClickedButton24()
{
	// TODO: Add your control notification handler code here

	handler->SetCollectSampleDataSignalAtThisTime(3,5,1);

}


void CMediaDlg::OnBnClickedButton25()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter2",L"LOW",L"FT");
}


void CMediaDlg::OnBnClickedButton26()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter2",L"LOW",L"RT");
}


void CMediaDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CMediaDlg::OnBnClickedButton2711()
{
	// TODO: Add your control notification handler code here
	/*
	log4cpp::Appender* appender;
#ifdef LOG4CPP_HAVE_SYSLOG
	log4cpp::SyslogAppender* syslogAppender;

	syslogAppender = new log4cpp::SyslogAppender("syslog", "log4cpp");
#else
	log4cpp::Appender* syslogAppender;

	syslogAppender = new log4cpp::OstreamAppender("syslogdummy", &std::cout);
#endif

	if (1) {
		//		appender = new log4cpp::OstreamAppender("default", &std::cout);
		appender = new log4cpp::FileAppender("default", "my.txt");

	} else {
		//appender = new log4cpp::FileAppender("default", argv[1]);
	}

	syslogAppender->setLayout(new log4cpp::BasicLayout());
	appender->setLayout(new log4cpp::BasicLayout());

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.addAppender(syslogAppender);
	root.setPriority(log4cpp::Priority::ERROR);

	log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));
	sub1.addAppender(appender);

	log4cpp::Category& sub2 = log4cpp::Category::getInstance(std::string("sub1.sub2"));

	log4cpp::NDC::push(std::string("ndc1"));

	std::cout << " root prio = " << root.getPriority() << std::endl;
	std::cout << " sub1 prio = " << sub1.getPriority() << std::endl;
	std::cout << " sub2 prio = " << sub2.getPriority() << std::endl;

	root.error("root error");
	root.warn("root warn");
	sub1.error("sub1 error");
	sub1.warn("sub1 warn");
	sub2.error("sub2 error");
	sub2.warn("sub2 warn");

	sub1.setPriority(log4cpp::Priority::INFO);
	std::cout << " root prio = " << root.getPriority() << std::endl;
	std::cout << " sub1 prio = " << sub1.getPriority() << std::endl;
	std::cout << " sub2 prio = " << sub2.getPriority() << std::endl;

	std::cout << "priority info" << std::endl;
	root.error("root error");
	root.warn("root warn");
	sub1.error("sub1 error");
	sub1.warn("sub1 warn");
	sub2.error("sub2 error");
	sub2.warn("sub2 warn");

	sub2.warnStream() << "streamed warn";

	sub2 << log4cpp::Priority::WARN << "warn2" << " warn3" 
		<< log4cpp::eol << " warn4";

	{
		for(int i = 0; i < 10000; i++) {
			char ndc2[20];
			sprintf(ndc2, "i=%d", i);
			log4cpp::NDC::push(ndc2);
			sub1.info("%s%d", "i = ", i);
			if ((i % 10) == 0) {
				sub1.log(log4cpp::Priority::NOTICE, "reopen log");
				if (log4cpp::Appender::reopenAll()) {
					sub1.info("log reopened");
				} else {
					sub1.warn("could not reopen log");
				}
			}
#ifndef WIN32
			sleep(1);
#endif
			log4cpp::NDC::pop();
		}
	}
	*/
}
//트레이 아이콘을 클릭했을 때의 메시지 핸들러
long CMediaDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	m_myTray.ProcTrayMsg(GetSafeHwnd(), wParam, lParam);
	return 0;
}

//트레이 아이콘 팝업메뉴의 종료 메뉴 메시지 핸들러
void CMediaDlg::OnAppExit(void)
{
	m_myTray.DelTrayIcon(GetSafeHwnd());
	CDialog::OnCancel();
}

//트레이 아이콘 보이기/숨기기 메뉴 메시지 핸들러
void CMediaDlg::OnDialogShow(void)
{

	if(!_invisible_flag) ShowWindow(false);	//보이는 상태라면 숨기고
	else ShowWindow(true);			//숨겨진 상태라면 보이게

	_invisible_flag = !_invisible_flag;
	m_myTray.m_bHide = _invisible_flag;
}

void CMediaDlg::OnBnClickedRadioRscript()
{
	// TODO: Add your control notification handler code here
}


void CMediaDlg::OnBnClickedButton28()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter3",L"LOW",L"FT");
}


void CMediaDlg::OnBnClickedButton29()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter3",L"LOW",L"RT");
}


void CMediaDlg::OnBnClickedButton30()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter4",L"LOW",L"FT");
}


void CMediaDlg::OnBnClickedButton31()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter3",L"HIGH",L"FT");
}


void CMediaDlg::OnBnClickedButton32()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter3",L"HIGH",L"RT");
}


void CMediaDlg::OnBnClickedButton33()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter2",L"HIGH",L"FT");
}


void CMediaDlg::OnBnClickedButton34()
{
	// TODO: Add your control notification handler code here
	if(handler->IsRunning())
	{
		OutputDebugString(L"기계 동작 중\r\n");
		return;
	}
	handler->LoadProjectAndFilterTuning(L"roswin_filter2",L"HIGH",L"RT");
}
